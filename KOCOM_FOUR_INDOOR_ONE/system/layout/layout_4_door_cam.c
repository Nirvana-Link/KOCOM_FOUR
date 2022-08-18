#include"ui_api.h"
#include"os_sys_api.h"
#include"layout_base.h"
#include"layout_common.h"
#include"ak_common.h"
#include"layout_cam_button.h"
#include"audio_play_api.h"
#include"l_audio_play.h"
#include"l_user_config.h"
#include"l_monitor.h"
#include"l_record.h"
#include"l_cctv_conf.h"
#include"l_network.h"
#include"l_hardware_control.h"
#include"sensor_video_api.h"
#include"tuya_sdk_api.h"
#include"tuya_sdk.h"
#include"l_absent_queue.h"
#include"l_absent_ev.h"
#include"debug.h"

#define      CURR_OUTDOOR         (monitor_channel_get() == CAM1 ? DEVICE_OUTDOOR_1 : DEVICE_OUTDOOR_2)

static bool is_camera_no_signal = false;
static  int curr_second = 0;//监控剩余时间
static  int camera_action = NOT;//监控动作，ANSWER: 接听    DOOR2:开门    NOT：等待
bool Is_answer_status = false;//是否已接听
static bool is_attr_window_open = false;//属性调节窗口是否打开
static int source_online_count = 0;//视频设备在线数量
static bool video_display_normal = false;

#ifdef Ste_PQ
static bool is_pq_bg_dis = false;
#endif



static void layout_door_cam_enter(void);
void monit_interface(l_user_config user_conf);
static void action_source_button_up(const void *arg);
static void camera_no_signal_display(bool is_erase);


static void monitor_tuya_appp_busy_display(void) {
    position pos = {{0, 0}, {1024, 600}};
    icon icon;
    resource res = resource_get(ROM_R_4_DOOR_CAM_APP_LINE_PNG);
    icon_init(&icon, &pos, &res);
    icon_display(&icon);

    pos = (position) {{112, 432},
                      {800, 48}};
    char *str[LANGUAGE_TOTAL] = {"App is monitoring…","모바일 앱에서 모니터링 중.","La aplicación está monitoreando…","التنطبيق قيد المراقبة ","Ứng dụng đang giám sát…","Приложение активно…"};
    text text;
    text_init(&text, &pos, 24);
    text.def_language = language_get();
    text_display(&text,str[language_get()]);
}


/* 操作-提示-显示 */
static void action_hint_display(enum btn_string_id str,int count)
{
    static int left_display_count = 0;
    static int right_display_count = 0;
    static bool left_hint_display = false;
    static bool right_hint_display = false;
    if(str == STR_CAM_BUSY)
    {
        left_display_count = count;
    }
    else if(str == STR_DOOR_OPENED)
    {
        right_display_count = count;
    }
    // DEBUG_LOG("right_display_count =================>%d      right_hint_display :%d\n\r",right_display_count,right_hint_display);
    // DEBUG_LOG("left_display_count =================>%d      left_hint_display :%d\n\r",left_display_count,left_hint_display);
    if(left_display_count == 0 && right_display_count == 0 && left_hint_display == false && right_hint_display == false)
        return;

    if(str != STR_TOTAL)
    {
        position pos = {{600,415},{150,60}};
        if(str == STR_CAM_BUSY && left_hint_display == false)
        {
            pos.point.x = 260;
            left_hint_display = true;
        }
        else if(str == STR_DOOR_OPENED && right_hint_display == false)
        {
            right_hint_display = true;
        }
        else
        {
            return;
        }

        button door_opened;
        resource res = resource_get(ROM_R_4_DOOR_CAM_DOOR_OPENED_PNG);
        button_init(&door_opened,&pos,NULL);
        button_icon_init(&door_opened,&res);
        button_text_init(&door_opened,btn_str(str),font_size(CAM_DOOR));
        door_opened.font_color = BLUE;
        door_opened.erase = true;
        door_opened.font_align = CENTER_MIDDLE;
        // door_opened.font_offset.y = -5;
        button_display(&door_opened);    
    }
    else 
    {
        if(left_display_count  == 0 && left_hint_display)
        {
            left_hint_display = false;
            position pos = {{260,415},{150,60}};
            gui_erase(&pos,0x00);
        }
        if(right_display_count  == 0 && right_hint_display)
        {
            right_hint_display = false;
            position pos = {{600,415},{150,60}};
            gui_erase(&pos,0x00);
        }
    }

    if(left_hint_display)
        left_display_count --;

    if(right_hint_display)
        right_display_count --;
}

/* 监控-属性调节-背景显示 
    Button :属性按钮
    Pq_Bg : 是否显示PQ设置背景(若无PQ设置功能即该参数无意义)*/
#define ATTR_LOCAL_BRI 0
#define ATTR_LOCAL_RING 1
#define ATTR_LOCAL_VOL 2
#define ATTR_OUTDOOR_RING 3
#define ATTR_OUTDOOR_VOL 4
icon attr_bg[5];
static void monit_attr_bg_init(void){
    resource res_local_b = resource_get(ROM_R_4_DOOR_CAM_SET_LITTLE_B_BG_PNG);
    resource res_local_v = resource_get(ROM_R_4_DOOR_CAM_SET_LOCAL_PNG);
    resource res_outdoor_v = resource_get(ROM_R_4_DOOR_CAM_SET_OUTDOOR_PNG);
    position pos_b = {{30,127},{50,401}};
    position pos_r = {{944,127},{50,401}};
    position pos_u = {{847,127},{50,401}};
    icon_init(&attr_bg[ATTR_LOCAL_BRI],&pos_b,&res_local_b);
    attr_bg[ATTR_LOCAL_BRI].png_buffer = true;

    icon_init(&attr_bg[ATTR_LOCAL_RING],&pos_r,&res_local_v);
    attr_bg[ATTR_LOCAL_RING].png_buffer = true;    

    icon_init(&attr_bg[ATTR_LOCAL_VOL],&pos_r,&res_local_v);
    attr_bg[ATTR_LOCAL_VOL].png_buffer = true;    

    icon_init(&attr_bg[ATTR_OUTDOOR_RING],&pos_u,&res_outdoor_v);
    attr_bg[ATTR_OUTDOOR_RING].png_buffer = true;    

    icon_init(&attr_bg[ATTR_OUTDOOR_VOL],&pos_u,&res_outdoor_v);
    attr_bg[ATTR_OUTDOOR_VOL].png_buffer = true;    
}
#if 0

#else
static void monit_attr_bg_display(button *Button){
    int cam_set = SequenceSearch(CAM_SET,Button,CAM_SET_TOTAL);
     switch (cam_set)
    {
        case LOCAL_BRI_ADJUST_BUTTON:
                    attr_bg[ATTR_LOCAL_BRI].erase = true;
                    icon_display(&attr_bg[ATTR_LOCAL_BRI]);
            break;

        case  LOCAL_VOL_ADJUST_BUTTON:
                if(Is_answer_status == false){
                    attr_bg[ATTR_LOCAL_RING].erase = true;
                    icon_display(&attr_bg[ATTR_LOCAL_RING]);
                }
                else{
                    attr_bg[ATTR_LOCAL_VOL].erase = true;
                    icon_display(&attr_bg[ATTR_LOCAL_VOL]);
                }
            break;

        case  OUTDOOR_VOL_ADJUST_BUTTON:
                if(Is_answer_status == false){
                    attr_bg[ATTR_OUTDOOR_RING].erase = true;
                    icon_display(&attr_bg[ATTR_OUTDOOR_RING]);
                }
                else{
                    attr_bg[ATTR_OUTDOOR_VOL].erase = true;
                    icon_display(&attr_bg[ATTR_OUTDOOR_VOL]);
                }
            break;
        default:
            break;
    }   
}
#endif
icon attr_adjust[CAM_SET_TOTAL];
/* 监控-属性量度条-显示 
    Button :属性按钮*/
static bool monit_attr_meas_display(enum cam_set_button Button)
{
#ifdef Set_PQ
        position pos_s = {{Button->pos.point.x+9,446},{38,25}};
        draw_rect(&pos_s,BLACK);       //显示属性具体值的背景

        if(is_pq_bg_dis && (Button != &CAM_SET[LOCAL_VOL_ADJUST_BUTTON])){
            position pos = {{Button->pos.point.x+24,(Button->pos.point.y+Button->icon_offset.y+30)},{5,212- Button->icon_offset.y}};           
            draw_rect(&pos,BLUE);
        }
        else
#endif
        {
            position pos = {{attr_adjust[Button].pos.point.x + 15,attr_adjust[Button].pos.point.y + 25},{5,467 - attr_adjust[Button].pos.point.y}};
            // DEBUG_LOG("Button : %d----------------------------> %d \n",Button,attr_adjust[Button].pos.point.x);
            draw_rect(&pos,WHITE);
        }
        return true;
}

static void attr_outdoor_vol_adjust_init(){
 
    position pos= {{855,200},{35,35}};
    resource res = resource_get(ROM_R_4_DOOR_CAM_SILDE_ADJUST_PNG);
    icon_init(&attr_adjust[OUTDOOR_VOL_ADJUST_BUTTON],&pos,&res);
    attr_adjust[OUTDOOR_VOL_ADJUST_BUTTON].png_buffer = true;
    // icon_display(&attr_adjust[LOCAL_VOL_ADJUST_BUTTON]);
}

static void attr_vol_adjust_init(){
 
    position pos= {{952,200},{35,35}};
    resource res = resource_get(ROM_R_4_DOOR_CAM_SILDE_ADJUST_PNG);
    icon_init(&attr_adjust[LOCAL_VOL_ADJUST_BUTTON],&pos,&res);
    attr_adjust[LOCAL_VOL_ADJUST_BUTTON].png_buffer = true;
    // icon_display(&attr_adjust[LOCAL_VOL_ADJUST_BUTTON]);
}

static void attr_bri_adjust_init(){

    position pos= {{38,200},{35,35}};
    resource res = resource_get(ROM_R_4_DOOR_CAM_SILDE_ADJUST_PNG);
    icon_init(&attr_adjust[LOCAL_BRI_ADJUST_BUTTON],&pos,&res);
    attr_adjust[LOCAL_BRI_ADJUST_BUTTON].png_buffer = true;
    // icon_display(&attr_adjust[LOCAL_BRI_ADJUST_BUTTON]);
}

/* 监控-属性全效果-显示 
    Button :属性按钮*/
static void monit_attr_but_display(enum cam_set_button Button){
    monit_attr_bg_display(&CAM_SET[Button]);      
    if((Button == LOCAL_VOL_ADJUST_BUTTON)){
        if(Is_answer_status == false){
            attr_adjust[LOCAL_VOL_ADJUST_BUTTON].pos.point.y = 200 + (6-user_data_get()->SOUR_PRO[monitor_channel_get()].sound.call_vol)*55;
        }else{
            attr_adjust[LOCAL_VOL_ADJUST_BUTTON].pos.point.y = 200 + (6-user_data_get()->SOUR_PRO[monitor_channel_get()].sound.talk_vol)*55;
        }        
    }else if(Button == OUTDOOR_VOL_ADJUST_BUTTON){
        if(Is_answer_status == false){
            // DEBUG_LOG("outdoor_call_vol>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d\n\r",user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_call_vol);
            attr_adjust[OUTDOOR_VOL_ADJUST_BUTTON].pos.point.y = 200 + (6-user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_call_vol)*55;
        }else{
            // DEBUG_LOG("outdoor_talk_vol>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d\n\r",user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_talk_vol);
            // DEBUG_LOG("outdoor_talk_vol>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d\n\r\n\r",user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_talk_vol);
            attr_adjust[OUTDOOR_VOL_ADJUST_BUTTON].pos.point.y = 200 + (6-user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_talk_vol)*55;
        }        
    }else if(Button == LOCAL_BRI_ADJUST_BUTTON){
        // DEBUG_LOG("bri>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d\n\r",user_data_get()->SOUR_PRO[monitor_channel_get()].bri);
        attr_adjust[LOCAL_BRI_ADJUST_BUTTON].pos.point.y = 200 + (9 - user_data_get()->SOUR_PRO[monitor_channel_get()].bri)*33;
    }

    monit_attr_meas_display(Button); 
    // button_display(Button);
    icon_display(&attr_adjust[Button]);
}


/* 监控-属性按键-滑动-显示
    Button :属性按钮
    slide_y: 手指滑动当前y坐标*/
static void monit_attr_slide_display(button *Button,int slide_y)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    /*滑动超出滑动有效范围,及返回,无效滑动*/
    if(slide_y > 475 ){
        slide_y = 475;
    }else if(slide_y < 200 ){
        slide_y = 200; 
    }
    int but = CAM_SET_TOTAL;
    
    if((Button == &CAM_SET[LOCAL_BRI_ADJUST_BUTTON]))
    {
        int motion_vol = abs(slide_y - 200)/33;
        if(user_data_get()->SOUR_PRO[monitor_channel_get()].bri == (9 - motion_vol)) 
            return;
        user_data_get()->SOUR_PRO[monitor_channel_get()].bri = 9 - motion_vol;// 视频背光亮度最低为3，因此滑动控制范围设置8个等级
        attr_adjust[LOCAL_BRI_ADJUST_BUTTON].pos.point.y = 200 + motion_vol*33;
        // DEBUG_LOG("user_default_sour : %d    BL:%d\n\r",monitor_channel_get(),user_data_get()->SOUR_PRO[monitor_channel_get()].bri);
        // set_lcd_brightness(user_data_get()->SOUR_PRO[monitor_channel_get()].bri);   
        brightness_get_vol(user_data_get()->SOUR_PRO[monitor_channel_get()].bri);
        but = LOCAL_BRI_ADJUST_BUTTON;
    }
    else if((Button == &CAM_SET[LOCAL_VOL_ADJUST_BUTTON]))
    {   
        int motion_vol = abs(slide_y - 200)/55;
        but = LOCAL_VOL_ADJUST_BUTTON;
  
        if(Is_answer_status == false){
            /* 0-5|1-6  对应音量等级*/
            // static int prev_call_volume = 0; //上一次音量大小
            motion_audio_set_volume((6 - motion_vol)*TONE_VOLUME_INTERVAL+TONE_MIN_VOLUME ,
                                                    user_data_get()->SOUR_PRO[monitor_channel_get()].sound.call_mel,
                                                    user_data_get()->SOUR_PRO[monitor_channel_get()].sound.speak_sen,
                                                    true);
            // printf("call_vol =[==============================>>%d\n\r",user_data_get()->SOUR_PRO[monitor_channel_get()].sound.call_vol);
            if(user_data_get()->SOUR_PRO[monitor_channel_get()].sound.call_vol == (6 - motion_vol)) 
                return;

            user_data_get()->SOUR_PRO[monitor_channel_get()].sound.call_vol = 6 - motion_vol;             
                                                    
        }else{
            /* 0-5|1-6  对应音量等级*/
            // static int prev_talk_volume = 0; //上一次音量大小     
            
            if(user_data_get()->SOUR_PRO[monitor_channel_get()].sound.talk_vol == (6 - motion_vol)) 
                return; 

            user_data_get()->SOUR_PRO[monitor_channel_get()].sound.talk_vol = 6 - motion_vol;
            motion_audio_set_volume(user_data_get()->SOUR_PRO[monitor_channel_get()].sound.talk_vol*TALK_VOLUME_CAMERA+TALK_MIN_VOLUME_CAMERA,0, user_data_get()->SOUR_PRO[monitor_channel_get()].sound.speak_sen,false);

            
        }
    }
    else if(Button == &CAM_SET[OUTDOOR_VOL_ADJUST_BUTTON]){
        int motion_vol = abs(slide_y - 200)/55;
        but = OUTDOOR_VOL_ADJUST_BUTTON;
  
        if(Is_answer_status == false){
            /* 0-6|1-6  对应音量等级*/
            // static int prev_call_volume = 0; //上一次音量大小
            if(user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_call_vol == (6 - motion_vol))
                return;

            user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_call_vol = 6 - motion_vol;
            // DEBUG_LOG("OUTDOOR_CALL_VOL>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d\n\r",user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_call_vol);

            
       
        }else{
            /* 0-2|1-3  对应音量等级*/
            // static int prev_talk_volume = 0; //上一次音量大小     
            if(user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_talk_vol == (6 - motion_vol)) 
                return; 
            // DEBUG_LOG("OUTDOOR_TALK_VOL>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d\n\r",user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_talk_vol);
            user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_talk_vol = 6 - motion_vol;

        }
    }
    monit_attr_but_display(but);
}

static void attr_open_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s       is_attr_window_open:%d        user_default_sour:%d\n\r",__func__, is_attr_window_open,  monitor_channel_get() );
    if(!is_attr_window_open){
        // DEBUG_LOG("11111111\n");
        is_attr_window_open = true;
#ifdef Set_PQ
        button_start_using(&CAM_SET[EXTEND_BUTTON]);
#endif
    if(monitor_channel_get() < CCTV1){
        // DEBUG_LOG("user_data_get()->SOUR_PRO[%d].sound.talk_vol : %d",monitor_channel_get(),user_data_get()->SOUR_PRO[monitor_channel_get()].sound.talk_vol);
        monit_attr_but_display(LOCAL_VOL_ADJUST_BUTTON);
        button_enable(&CAM_SET[LOCAL_VOL_ADJUST_BUTTON]);
        monit_attr_but_display(OUTDOOR_VOL_ADJUST_BUTTON);
        button_enable(&CAM_SET[OUTDOOR_VOL_ADJUST_BUTTON]);
        }

        monit_attr_but_display(LOCAL_BRI_ADJUST_BUTTON);
        button_enable(&CAM_SET[LOCAL_BRI_ADJUST_BUTTON]);
    }
    else{
        is_attr_window_open = false;
        // CAM_SET[LOCAL_BRI_ADJUST_BUTTON].pos.point.x = 30;
        gui_erase(&attr_bg[ATTR_LOCAL_BRI].pos,0x00);
        gui_erase(&attr_bg[ATTR_LOCAL_RING].pos,0x00);
        gui_erase(&attr_bg[ATTR_OUTDOOR_RING].pos,0x00);
#ifdef Set_PQ
        button_disable(&CAM_SET[EXTEND_BUTTON]);
#endif
        button_disable(&CAM_SET[LOCAL_BRI_ADJUST_BUTTON]);
        button_disable(&CAM_SET[LOCAL_VOL_ADJUST_BUTTON]);
        button_disable(&CAM_SET[OUTDOOR_VOL_ADJUST_BUTTON]);
    }
}
/* 监控-属性调节入口按钮-定义 */
static void attr_open_button_init(void)
{
    position pos = {{150,200},{690,250}};
    button_init(&attr_open_button,&pos,attr_open_button_up);
}

static void attr_adjust_button_up(const void *arg){
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    if(Par_Button == &CAM_SET[OUTDOOR_VOL_ADJUST_BUTTON]){
        unsigned int net_send_arg1 = Is_answer_status == false ? user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_call_vol  : user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_talk_vol ;
        unsigned int net_send_arg2 = Is_answer_status == false ? 0x00:0x01;        
        network_cmd_param_set(CURR_OUTDOOR,NET_CMD_SET_CAMERA_VOL,net_send_arg1,net_send_arg2,4);
        net_common_send((CURR_OUTDOOR) ,
                                        NET_CMD_SET_CAMERA_VOL,
                                        net_send_arg1,
                                        net_send_arg2);
    }
                

}
static void attr_adjust_button_motion(const void *arg)
{
    // unsigned long long start  = os_get_ms();
    const point *cur_x = touch_current_point();

    // DEBUG_LOG("cur_x->y-------------->>>%d \n\r",cur_x->y);
    monit_attr_slide_display(Par_Button,cur_x->y);
    // DEBUG_LOG("------------------------->>>%llu \n\r",os_get_ms() - start);
}
/* 监控-属性调节按钮-定义 
    Button : 相关属性按钮
    pos : 按钮区域
    res : 按钮UI
    up : 松开回调函数指针
    motion : 滑动及按下回调函数指针*/
static void attr_adjust_button_init(button *Button,position *pos,touch_func_callback up,touch_func_callback motion)
{
    button_init(Button,pos,up);
    // button_icon_init(Button,res);
    // Button->bg_color = BLUE;
    Button->motion = motion;
    Button->down = motion;
    // Button->icon_align = CENTER_TOP;
    //button_text_init(Button,btn_str(STR_AM),font_size(CAM_DOOR_SET));
    Button->font_color = 0xffffff;    
    Button->font_align = CENTER_BOTTOM;
    Button->font_offset.y = -15;
}
/* 监控-属性调节按钮-列表 */


static void attr_adjust_button_list(void)
{
    position pos= {{941,210},{55,370}};
    // resource res =resource_get(ROM_R_4_DOOR_CAM_SILDE_ADJUST_PNG);
    attr_adjust_button_init(&CAM_SET[LOCAL_VOL_ADJUST_BUTTON],&pos,attr_adjust_button_up, attr_adjust_button_motion);
    CAM_SET[LOCAL_VOL_ADJUST_BUTTON].button_tone = -1;

    pos.point.x = 30;
    attr_adjust_button_init(&CAM_SET[LOCAL_BRI_ADJUST_BUTTON],&pos,attr_adjust_button_up, attr_adjust_button_motion);

    pos.point.x = 844;
    attr_adjust_button_init(&CAM_SET[OUTDOOR_VOL_ADJUST_BUTTON],&pos,attr_adjust_button_up, attr_adjust_button_motion);
#ifdef Set_PQ
    pos.point.x = 120;
    attr_adjust_button_init(&CAM_SET[CON_ADJUST_BUTTON],&pos,&res,attr_adjust_button_up, attr_adjust_button_motion);
    pos.point.x = 201;
    attr_adjust_button_init(&CAM_SET[SAT_ADJUST_BUTTON],&pos,&res,attr_adjust_button_up, attr_adjust_button_motion);
    pos.point.x = 282;
    attr_adjust_button_init(&CAM_SET[HUE_ADJUST_BUTTON],&pos,&res,attr_adjust_button_up, attr_adjust_button_motion);    
    pos.point.x = 363;
    attr_adjust_button_init(&CAM_SET[SHA_ADJUST_BUTTON],&pos,&res,attr_adjust_button_up, attr_adjust_button_motion);
#endif
}

#ifdef Set_PQ
static void enable_residue_adjust(void)
{
    CAM_SET[LOCAL_BRI_ADJUST_BUTTON].pos.point.x = 39;
    button_control_group(CAM_SET,LOCAL_BRI_ADJUST_BUTTON,CAM_SET_TOTAL,monit_attr_meas_display);
#ifdef Set_PQ
    button_control_group(CAM_SET,CON_ADJUST_BUTTON,CAM_SET_TOTAL,button_start_using);
#endif
    button_display(&CAM_SET[LOCAL_BRI_ADJUST_BUTTON]);
}
static void  set_extend_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    position pos =  {{29,112},{500,380}};
    gui_erase(&pos,0x00);
    is_pq_bg_dis = true;
    button_start_using(&CAM_SET[EXTEND_REFRESH_BUTTON]);
    button_start_using(&CAM_SET[EXTEND_CLOSE_BUTTON]);
    button_disable(&attr_open_button);
    button_disable(&CAM_SET[EXTEND_BUTTON]);
    monit_attr_bg_display(NULL);
    enable_residue_adjust();

}
static void set_extend_button_init(void)
{
    position pos = {{100,114},{40,40}};
    button_init(&CAM_SET[EXTEND_BUTTON],&pos,set_extend_button_up);
    resource res = resource_get(ROM_R_4_DOOR_CAM_SET_EXTEND_PNG);
    button_icon_init(&CAM_SET[EXTEND_BUTTON],&res);
}

static void   set_extend_close_up(const void *arg)
{
    is_pq_bg_dis = false;
    CAM_SET[LOCAL_BRI_ADJUST_BUTTON].pos.point.x = 30;
    button_control_group(CAM_SET,EXTEND_CLOSE_BUTTON,CAM_SET_TOTAL,button_disable);    
    button_enable(&attr_open_button);
}
static void   set_extend_close_button_leave_down(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    button_refresh(&CAM_SET[EXTEND_CLOSE_BUTTON],ROM_R_4_DOOR_CAM_CLOSE_PNG,ROM_R_4_DOOR_CAM_CLOSE_PNG_SIZE,true);
    is_attr_window_open = false;
    position pos =  {{25,112},{1000,380}};
    gui_erase(&pos,0x00);
}
static void   set_extend_close_button_down(const void *arg)
{
    button_refresh(&CAM_SET[EXTEND_CLOSE_BUTTON],ROM_R_4_DOOR_CAM_CLOSE_SELECT_PNG,ROM_R_4_DOOR_CAM_CLOSE_SELECT_PNG_SIZE,true);
}
static void   set_extend_close_init(void)
{
    position pos = {{445,114},{40,40}};
    button_init(&CAM_SET[EXTEND_CLOSE_BUTTON],&pos,set_extend_close_up);
    resource res = resource_get(ROM_R_4_DOOR_CAM_CLOSE_PNG);
    button_icon_init(&CAM_SET[EXTEND_CLOSE_BUTTON],&res);
    CAM_SET[EXTEND_CLOSE_BUTTON].down = set_extend_close_button_down;
    CAM_SET[EXTEND_CLOSE_BUTTON].leave_down = set_extend_close_button_leave_down;
}

static void   set_extend_refresh_up(const void *arg){}
static void   set_extend_refresh_button_leave_down(const void *arg)
{
    button_refresh(&CAM_SET[EXTEND_REFRESH_BUTTON],ROM_R_4_DOOR_CAM_REFRESH_PNG,ROM_R_4_DOOR_CAM_REFRESH_PNG_SIZE,true);
}
static void   set_extend_refresh_down(const void *arg)
{
    button_refresh(&CAM_SET[EXTEND_REFRESH_BUTTON],ROM_R_4_DOOR_CAM_REFRESH_SELECT_PNG,ROM_R_4_DOOR_CAM_REFRESH_SELECT_PNG_SIZE,true);
}
static void   set_extend_refresh_init(void)
{
    position pos = {{445,173},{40,40}};
    button_init(&CAM_SET[EXTEND_REFRESH_BUTTON],&pos,set_extend_refresh_up);
    resource res = resource_get(ROM_R_4_DOOR_CAM_REFRESH_PNG);
    button_icon_init(&CAM_SET[EXTEND_REFRESH_BUTTON],&res);
    CAM_SET[EXTEND_REFRESH_BUTTON].down = set_extend_refresh_down;
    CAM_SET[EXTEND_REFRESH_BUTTON].leave_down = set_extend_refresh_button_leave_down;
}
#endif
static void monitor_shoot_btn_display(void){
     if(is_record_pictrue_ing()== true)
	{
		resource res =  resource_get(ROM_R_4_DOOR_CAM_SHOOT_SELECT_PNG);
		button_icon_init(&CAM_ACTION[ACTION_SHOOT_BUTTON], &res);
	}
    else
    {
		resource res =  resource_get(ROM_R_4_DOOR_CAM_SHOOT_PNG);
		button_icon_init(&CAM_ACTION[ACTION_SHOOT_BUTTON], &res);
    }
    CAM_ACTION[ACTION_SHOOT_BUTTON].erase = true;
    button_display(&CAM_ACTION[ACTION_SHOOT_BUTTON]);
}
static void action_shoot_button_up(const void *arg){
    DEBUG_LOG("--------------->>>%s start\n\r",__func__);
    if(record_pictrue_start((arg == NULL)?REC_MODE_AUTO:REC_MODE_MANUAL,monitor_channel_get()) == false){
        // goto btn_display;
        return;
    } 


    // record_pictrue_start((arg == NULL)?REC_MODE_AUTO:REC_MODE_MANUAL,monitor_channel_get());
    // btn_display:
    monitor_shoot_btn_display();
    // DEBUG_LOG("--------------->>>%s end\n\r",__func__);
}
/* 摄像按钮-定义 */
static void action_shoot_button_init(void)
{
    position pos = {{464,488},{80,80}};
    button_init(&CAM_ACTION[ACTION_SHOOT_BUTTON],&pos,action_shoot_button_up);
    resource res = resource_get(ROM_R_4_DOOR_CAM_SHOOT_PNG);
    button_icon_init(&CAM_ACTION[ACTION_SHOOT_BUTTON],&res);
}
#if SD_CARD
static void monitor_record_video_btn_display(void)
{
    if(is_sdcard_insert()){
        if(is_record_video_ing()== true)
        {
            resource res=  resource_get(ROM_R_4_DOOR_CAM_REC_VIDEO_ON_PNG);
            button_icon_init(&CAM_ACTION[ACTION_RECODE_BUTTON], &res);
            button_disable(&CAM_ACTION[ACTION_CAM1_BUTTON]);
        }
        else
        {
            resource res=  resource_get(ROM_R_4_DOOR_CAM_REC_VIDEO_PNG);
            button_icon_init(&CAM_ACTION[ACTION_RECODE_BUTTON], &res);
            button_enable(&CAM_ACTION[ACTION_CAM1_BUTTON]);
        }
        CAM_ACTION[ACTION_RECODE_BUTTON].erase = true;
        button_display(&CAM_ACTION[ACTION_RECODE_BUTTON]); 
    }
    else{
            resource res=  resource_get(ROM_R_4_DOOR_CAM_REC_DISABLE_PNG);
            button_icon_init(&CAM_ACTION[ACTION_RECODE_BUTTON], &res);
            button_display(&CAM_ACTION[ACTION_RECODE_BUTTON]); 
            //button_disable(&CAM_ACTION[ACTION_RECODE_BUTTON]); 
    }
}
static void action_recode_button_up(const void *arg){
    DEBUG_LOG("\n\r------------------------------------------->>>%s \n\r",__func__);
    if(!is_sdcard_insert())
        return;

    if(is_record_video_ing() == true){
		record_video_stop(0x00);
        if(Is_answer_status == false){
        audio_stream_close(RECEIVE_STREAM);            
        }
	}
	else{
        if(Is_answer_status == false)
        audio_stream_open(monitor_channel_get()== CAM1?0:1,false);

		record_video_start(REC_MODE_MANUAL,0x00, monitor_channel_get());
	}

    monitor_record_video_btn_display();
}

/* 录像按钮-定义 */
static void action_recode_button_init(void)
{
    position pos = {{782,504},{80,80}};
    button_init(&CAM_ACTION[ACTION_RECODE_BUTTON],&pos,action_recode_button_up);
    resource res = resource_get(ROM_R_4_DOOR_CAM_REC_VIDEO_PNG);
    button_icon_init(&CAM_ACTION[ACTION_RECODE_BUTTON],&res);
}
#endif
// static void call_cam_hint(int call_id,bool is_erase,int pos_x);
static void action_answer_button_up(const void *arg){}
static void action_answer_button_down(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);     
    if(camera_action == ANSWER){
        return;
    }

    CAMERA_STATUS.connectable[monitor_channel_get() - 1] = NONE_STATUS;              


    Audio_play_stop();
    camera_action = ANSWER;
    return;
}
/* 接听按钮-定义 */
static void action_answer_button_init(void)
{
    position pos = {{295,488},{80,80}};
    button_init(&CAM_ACTION[ACTION_ANSWER_BUTTON],&pos,action_answer_button_up);
    resource res = resource_get(ROM_R_4_DOOR_CAM_ANSWER_PNG);
    button_icon_init(&CAM_ACTION[ACTION_ANSWER_BUTTON],&res);
    CAM_ACTION[ACTION_ANSWER_BUTTON].down = action_answer_button_down;
}

static void action_door2_display(bool open){
    if(open == true)
        button_refresh(&CAM_ACTION[ACTION_DOOR2_BUTTON],ROM_R_4_DOOR_CAM_DOOR2_PNG,ROM_R_4_DOOR_CAM_DOOR2_PNG_SIZE,true);
    else
        button_refresh(&CAM_ACTION[ACTION_DOOR2_BUTTON],ROM_R_4_DOOR_CAM_DOOR1_PNG,ROM_R_4_DOOR_CAM_DOOR1_PNG_SIZE,true);
}
static void action_door2_button_up(const void *arg){}
static void action_door2_button_leave_down(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    action_door2_display(false);
}

static void action_door2_button_down(const void *arg)
{
    camera_action = DOOR2;
    action_door2_display(true);
    DEBUG_LOG("--------------->>>%s end\n\r",__func__);
}
/* 开门按键-定义 */
static void action_door2_button_init(void)
{
    position pos = {{633,488},{80,80}};
    button_init(&CAM_ACTION[ACTION_DOOR2_BUTTON],&pos,action_door2_button_up);
    resource res = resource_get(ROM_R_4_DOOR_CAM_DOOR1_PNG);
    button_icon_init(&CAM_ACTION[ACTION_DOOR2_BUTTON],&res);
    CAM_ACTION[ACTION_DOOR2_BUTTON].down = action_door2_button_down;
    CAM_ACTION[ACTION_DOOR2_BUTTON].leave_down = action_door2_button_leave_down;
}

static void action_home_button_up(const void *arg){}
static void action_home_button_leave_down(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    button_refresh(&CAM_ACTION[ACTION_HOME_BUTTON],ROM_R_4_DOOR_CAM_EXIT_PNG,ROM_R_4_DOOR_CAM_EXIT_PNG_SIZE,true);
    app_layout_goto(&layout_home);
    return;
}

static void action_home_button_down(const void *arg)
{
    // DEBUG_LOG("--------------->>>%s start\n\r",__func__);
    button_refresh(&CAM_ACTION[ACTION_HOME_BUTTON],ROM_R_4_DOOR_CAM_EXIT_PRESS_PNG,ROM_R_4_DOOR_CAM_EXIT_PRESS_PNG_SIZE,true);
    DEBUG_LOG("--------------->>>%s end\n\r",__func__);
}
/* 主页按键-定义 */
static void action_home_button_init(void)
{
    position pos = {{30,30},{50,50}};
    button_init(&CAM_ACTION[ACTION_HOME_BUTTON],&pos,action_home_button_up);
    resource res = resource_get(ROM_R_4_DOOR_CAM_EXIT_PNG);
    button_icon_init(&CAM_ACTION[ACTION_HOME_BUTTON],&res);
    CAM_ACTION[ACTION_HOME_BUTTON].down = action_home_button_down;
    CAM_ACTION[ACTION_HOME_BUTTON].leave_down = action_home_button_leave_down;
}


int source_online_num_get(void)
{
    int online_count = 0;
    online_count = 0;
    for(network_device  dev_index= DEVICE_OUTDOOR_1;dev_index < DEVICE_TOTAL;dev_index++)
    {
        if(device_list_ip_get(dev_index) != NULL)
        {
            online_count ++;
        }
    }
    return online_count;
}

/* 输入源按钮UI-定义 
    is_on : 是否接通
    but : 输入源按钮*/
void source_res_int(bool is_on,enum cam_action_button but)
{

    if(device_list_ip_get(but + DEVICE_OUTDOOR_1) == NULL){
        resource res = resource_get(ROM_R_4_DOOR_CAM_SOURCE_OFF_PNG);  
        button_icon_init(&CAM_ACTION[but],&res);
    }
    else
    {
        if(!is_on) {
            resource res = resource_get(but < ACTION_CCTV1_BUTTON ? ROM_R_4_DOOR_CAM_SOURCE_PNG : ROM_R_4_DOOR_CAM_CCTV_PNG);  
            button_icon_init(&CAM_ACTION[but],&res);
        }
        else{
            resource res = resource_get(but < ACTION_CCTV1_BUTTON ? ROM_R_4_DOOR_CAM_SOURCE_SELECT_PNG : ROM_R_4_DOOR_CAM_CCTV_ON_PNG);  
            button_icon_init(&CAM_ACTION[but],&res);
        }        
    }

    CAM_ACTION[but].erase = true;
}

void source_button_using(void)
{
    position pos = {{110,25},{790,70}};
    gui_erase(&pos,0x00);
    int online_count = source_online_count = source_online_num_get();

    int pos_offset = (4 - online_count)  *100;
    for(network_device  dev_index= DEVICE_OUTDOOR_1;dev_index < DEVICE_TOTAL && online_count != 0;dev_index++)
    {
        if(device_list_ip_get(dev_index) != NULL)
        {
            CAM_ACTION[dev_index - DEVICE_OUTDOOR_1].pos.point.x = 110 + pos_offset + (source_online_count - online_count) *200;
            CAM_ACTION[dev_index - DEVICE_OUTDOOR_1].erase = true;
            source_res_int(user_data_get()->user_default_sour == dev_index - DEVICE_INDOOR_ID4 ? true : false,dev_index - DEVICE_OUTDOOR_1);
            button_start_using(&CAM_ACTION[dev_index - DEVICE_OUTDOOR_1]);
            online_count --;
        }
        else
        {
            button_disable(&CAM_ACTION[dev_index - DEVICE_OUTDOOR_1]);
        }
    }
}

/* 输入源按钮位置-定义 */
void source_pos_int(void)
{
    int online_count = source_online_count = source_online_num_get();

    for(network_device  dev_index= DEVICE_OUTDOOR_1;dev_index < DEVICE_TOTAL && online_count != 0;dev_index++)
    {
        if(device_list_ip_get(dev_index) != NULL)
        {
            int pos_offset = (4 - online_count)  *100;
            CAM_ACTION[dev_index - DEVICE_OUTDOOR_1].pos.point.x = 110 + pos_offset;
            online_count --;
        }
    }
}


static void action_source_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s\n\r",__func__);

    int sour_index = SequenceSearch(CAM_ACTION,(Par_Button),ACTION_HOME_BUTTON)+CAM1/* 加CAM1值使得枚举 cam_action_button与枚举source_index对齐*/;
    DEBUG_LOG("sour_index-------------->>>%d \n\r",sour_index);
    // call_cam_hint(0,true,0);/*  */
    curr_second = user_data_get()->user_default_mode_conf.time_conf.mon_time;
    
    if(monitor_channel_get() != sour_index && (device_list_ip_get(sour_index + 3) != NULL)){//如果当前输入源改变
        is_camera_no_signal = false;
        // source_res_int(false,monitor_channel_get() - 1); 

        if(monitor_channel_get() < CCTV1){
            looup_time_set_param(CURR_OUTDOOR,Is_answer_status == false ? NET_CMD_END_CAMERA_ACK : NET_COMMON_CMD_OUTDOOR_LEISURE,3);
        }
        monitor_channel_set(sour_index);//先发送命令至被切视频，再转视频源
        
        camera_no_signal_display(monitor_open()/* monitor_switch(sour_index,Is_answer_status) */);

        if(is_attr_window_open){
            if(monitor_channel_get() < CCTV1)
            {
                monit_attr_but_display(OUTDOOR_VOL_ADJUST_BUTTON);
                monit_attr_but_display(LOCAL_VOL_ADJUST_BUTTON);                
                button_enable(&CAM_SET[OUTDOOR_VOL_ADJUST_BUTTON]);
                button_enable(&CAM_SET[LOCAL_VOL_ADJUST_BUTTON]);
            }
            else
            {
                gui_erase(&attr_bg[ATTR_LOCAL_RING].pos,0x00);
                gui_erase(&attr_bg[ATTR_OUTDOOR_RING].pos,0x00);
                button_disable(&CAM_SET[OUTDOOR_VOL_ADJUST_BUTTON]);
                button_disable(&CAM_SET[LOCAL_VOL_ADJUST_BUTTON]);
            }

            monit_attr_but_display(LOCAL_BRI_ADJUST_BUTTON);
        }
        set_lcd_brightness(user_data_get()->SOUR_PRO[monitor_channel_get()].bri); 

        Is_answer_status = false;
        monit_interface(user_conf);        
    }
    else
    {

    }
}
/* 所有-输入源按钮-定义 */
static void source_button_init(void)
{
    resource res = resource_get(ROM_R_4_DOOR_CAM_SOURCE_PNG);  
    resource res1 = resource_get(ROM_R_4_DOOR_CAM_CCTV_PNG);  
    position pos = {{90,27},{186,60}};      
     for(int index = ACTION_CAM1_BUTTON;index<ACTION_HOME_BUTTON;index++)
     {
        button_init(&CAM_ACTION[index],&pos,action_source_button_up);
        button_icon_init(&CAM_ACTION[index],index < ACTION_CCTV1_BUTTON ? &res : &res1);
        button_text_init(&CAM_ACTION[index],btn_str(STR_CAM1+index),23);
        CAM_ACTION[index].font_offset.x = 20;
        CAM_ACTION[index].font_align = CENTER_MIDDLE;
        pos.point.x +=  220;
     }
     CAM_ACTION[ACTION_CAM1_BUTTON].png_buffer = true;
}

static void action_hangup_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    // curr_second = 0;
    DEBUG_LOG("SEND NET_COMMON_CMD_OUTDOOR_LEISURE  %d\n\r",__LINE__);
    looup_time_set_param(CURR_OUTDOOR,NET_COMMON_CMD_OUTDOOR_LEISURE,5);
    app_layout_goto(&layout_home);
}
/* 挂断按钮-定义 */
static void action_hangup_button_init(void)
{
    position pos = {{295,488},{80,80}};
    button_init(&CAM_ACTION[ACTION_HANGUP_BUTTON],&pos,action_hangup_button_up);
    resource res = resource_get(ROM_R_4_DOOR_CAM_HANGUP_PNG);
    button_icon_init(&CAM_ACTION[ACTION_HANGUP_BUTTON],&res);
    CAM_ACTION[ACTION_HANGUP_BUTTON].erase = true;
}


static void camera_button_tone_control(bool mute){
    button_control_group(CAM_ACTION,ACTION_CAM1_BUTTON,ACTION_HOME_BUTTON,mute ? button_tone_mute : button_tone_open);
    button_control_group(CAM_ACTION,ACTION_DOOR2_BUTTON,ACTION_HANGUP_BUTTON,mute ? button_tone_mute : button_tone_open);
    button_control_group(CAM_SET,LOCAL_BRI_ADJUST_BUTTON,CAM_SET_TOTAL,mute ? button_tone_mute : button_tone_open);
    if(mute)
        button_tone_mute(&attr_open_button);
    else
        button_tone_open(&attr_open_button);

}
/* 接听界面 */
static void action_answering_layer(void)
{

    camera_action= NOT;
    Is_answer_status = true;              
    //输入源控件状态改变
#if SD_CARD
    CAM_ACTION[ACTION_RECODE_BUTTON].pos.point.x = 578;   
    //关闭接听等同区域控件，进入接听状态使能挂断及开门等控件
    button_control_group(CAM_ACTION,ACTION_ANSWER_BUTTON,ACTION_RECODE_BUTTON,button_disable);
    button_control_group(CAM_ACTION,ACTION_RECODE_BUTTON,ACTION_SET_TOTAL,button_start_using);
    // button_start_using(&CAM_ACTION[ACTION_HANGUP_BUTTON]);    
#else
    button_disable(&CAM_ACTION[ACTION_ANSWER_BUTTON]);
    button_control_group(CAM_ACTION,ACTION_DOOR2_BUTTON,ACTION_SET_TOTAL,button_start_using);
#endif
    camera_button_tone_control(Is_answer_status);
    button_display(&CAM_ACTION[monitor_channel_get() - 1]);
    action_door2_display(false);
    if(is_attr_window_open){
        monit_attr_but_display(LOCAL_VOL_ADJUST_BUTTON);
        monit_attr_but_display(OUTDOOR_VOL_ADJUST_BUTTON);
    }
}
/* 滚球滑动-效果 */
static void action_ball_roll(void)
{
    // DEBUG_LOG("------------------->%s   start\n\r",__func__);
    static int ball_silde_count = 0;//camera指令发送次数

    if(((ball_silde_count++) < 5)/* &&(camera_action != NOT) */)
    {
        if(camera_action == ANSWER){

            if(CAMERA_STATUS.connectable[monitor_channel_get() - 1] == NONE_STATUS)
                net_common_send((CURR_OUTDOOR) ,
                                            NET_CMD_ANSWER_CAMERA_REQUEST,
                                            180,
                                            monitor_channel_get());
        }
    }
    else
    {
        // DEBUG_LOG("%d-------------->>>%s \n\r",__LINE__,__func__);
        ball_silde_count = 0;
        if(camera_action == ANSWER)
        {
            if(CAMERA_STATUS.connectable[monitor_channel_get() - 1] == IDLE_STATUS){
                curr_second = 180;//接听后时间重置为180s
                                                  

                // call_cam_hint(0,true,0);
                action_answering_layer();
                if(audio_stream_status_get() == false && (device_list_ip_get((monitor_channel_get() == CAM1)? DEVICE_OUTDOOR_1 : DEVICE_OUTDOOR_2) != NULL) && get_monitor_open_flag())
                {
                    unsigned int net_send_arg1 = user_data_get()->SOUR_PRO[monitor_channel_get()].outdoor_talk_vol;
                    unsigned int net_send_arg2 = 0x01;    
                    network_cmd_param_set(CURR_OUTDOOR,NET_CMD_SET_CAMERA_VOL,net_send_arg1,net_send_arg2,2);
                    audio_stream_open(monitor_channel_get()== CAM1?0:1,
                                                                user_data_get()->SOUR_PRO[monitor_channel_get()].sound.talk_vol * TALK_VOLUME_CAMERA - 80 + TALK_MIN_VOLUME_CAMERA,
                                                                user_data_get()->SOUR_PRO[monitor_channel_get()].sound.speak_sen/* *2 */);
                    amp_enable(true);
                }
                return ;
            }else{
                    // DEBUG_LOG("CAM CAN NOT CONNECT----------------------------------------------->\n\r");
                    action_hint_display(STR_CAM_BUSY,3);
            }
            CAMERA_STATUS.connectable[monitor_channel_get() - 1] = NONE_STATUS;
        }
        else if(camera_action == DOOR2){
            int door_open_time = user_data_get()->user_default_mode_conf.time_conf.door_time;
            int display_time = 1 + (door_open_time > 5 ? door_open_time : door_open_time < 1 ? 1 : door_open_time);
            curr_second = door_open_time > 5 ? door_open_time + 1 : 5;
            action_hint_display(STR_DOOR_OPENED,display_time);
            // button_listens_destroy();
            start_door_unlock();

        }    
        camera_action = NOT;
    }
        // DEBUG_LOG("------------------->%s   end\n\r",__func__);
}

/* 监控-按键-显示
     is_CAM ：当前输入源是否为CAM，若是则显示CAM监控相关UI，反之显示CCTV相关UI*/
static void door_cam_button_display(bool is_CAM)
{
    // button_text_init(&CAM_ACTION[ACTION_CAM1_BUTTON],btn_str(STR_CAM1+monitor_channel_get()-1/* 输入源id从1开始，因此需要减1 */),24);   
    if(is_CAM){
#if SD_CARD
        button_control_group(CAM_ACTION,ACTION_ANSWER_BUTTON,ACTION_RECODE_BUTTON,button_display);       
#else
        action_door2_display(false);
        button_control_group(CAM_ACTION,ACTION_ANSWER_BUTTON,ACTION_SHOOT_BUTTON,button_display);
#endif
    }
    else
    {
        // button_display(&CAM_ACTION[ACTION_HANGUP_BUTTON]);
    }
    button_control_group(CAM_ACTION,ACTION_HOME_BUTTON,ACTION_ANSWER_BUTTON,button_display);    
#if SD_CARD
    monitor_record_video_btn_display();
#endif
    monitor_shoot_btn_display();
}
/* 监控-界面-使能按钮
     is_CAM ：当前输入源是否为CAM，若是则使能CAM监控相关按钮，反之使能CCTV相关按钮*/
static void door_cam_button_enable(bool is_CAM)
{
    // button_start_using(& CAM_ACTION[ACTION_CAM1_BUTTON]);   
    button_enable(&attr_open_button);          
    if(is_CAM){
        // button_enable(&CAM_ACTION[ACTION_CAM1_BUTTON]);
        button_control_group(CAM_ACTION,ACTION_HOME_BUTTON,ACTION_HANGUP_BUTTON,button_enable); 
    }
    else
    {
        // button_enable(&CAM_ACTION[ACTION_HANGUP_BUTTON]);
#if SD_CARD
        button_control_group(CAM_ACTION,ACTION_RECODE_BUTTON,ACTION_HANGUP_BUTTON,button_enable); 
#else
        // button_control_group(CAM_ACTION,ACTION_DOOR2_BUTTON,ACTION_HANGUP_BUTTON,button_enable); 
        button_enable(&CAM_ACTION[ACTION_SHOOT_BUTTON]);
#endif
    }
    button_control_group(CAM_ACTION,ACTION_HOME_BUTTON,ACTION_ANSWER_BUTTON,button_enable); 
}
/* 监控-待界面-显示 
    user_conf ：当前输入源，根据输入源进入对应监控界面*/
void monit_interface(l_user_config user_conf)
{
    if(monitor_enter_flag_get() == MONITOR_ENTER_CALL){
        monitor_enter_flag_set(MONITOR_ENTER_NONE);
        // call_cam_hint(monitor_channel_get() == CAM1 ?ACTION_CAM1_BUTTON : ACTION_CAM2_BUTTON,false,0);
    }
    //擦除接听区域的图像    
    position pos = {{280,488},{430,90}};
    gui_erase(&pos,0x00);         
    if(monitor_channel_get() > CAM2)
    {
        //将拍摄控件位置调整
        //挂断控件位置调整
        // //关闭接听等同区域控件，进入接听状态使能挂断及开门等控件 
#if SD_CARD
        CAM_ACTION[ACTION_RECODE_BUTTON].pos.point.x = 578;    
        button_control_group(CAM_ACTION,ACTION_ANSWER_BUTTON,ACTION_RECODE_BUTTON,button_disable);
#else
        button_control_group(CAM_ACTION,ACTION_ANSWER_BUTTON,ACTION_SHOOT_BUTTON,button_disable);
#endif
        button_disable(&CAM_ACTION[ACTION_HANGUP_BUTTON]);
        door_cam_button_display(false);
        door_cam_button_enable(false);

    }
    else
    {
#if SD_CARD
        CAM_ACTION[ACTION_RECODE_BUTTON].pos.point.x = 782;
#endif
        button_disable(&CAM_ACTION[ACTION_HANGUP_BUTTON]);
        door_cam_button_display(true);
        door_cam_button_enable(true);
    }

    source_button_using();
    button_disable(&CAM_ACTION[monitor_channel_get() - 1]);
    camera_button_tone_control(Is_answer_status);
}
extern  bool return_disturb_status(void);

static bool tuya_set_absent_mode(bool mode) {
    if (security_in_alarm_state() == true && mode == false) {
        user_data_get()->security.running = false;
        user_data_get()->security.Sensor1_open = false;
        user_data_get()->security.Sensor2_open = false;
        user_data_get()->security.dismissed = 0;
        user_data_get()->security.curr_trigger_id = 0;
        app_layout_goto(&layout_home);
    } else {
        if (mode == true) {
            // if (user_data_get()->security.Sensor1_open || user_data_get()->security.Sensor2_open) {
            //     user_data_get()->security.running = true;
            // } else {
                user_data_get()->security.Sensor1_open = true;
                user_data_get()->security.Sensor2_open = true;
                user_data_get()->security.running = true;
            // }
        } else {
            user_data_get()->security.running = false;
            user_data_get()->security.Sensor1_open = false;
            user_data_get()->security.Sensor2_open = false;
        }
    }
    int arg1 = user_data_get()->security.running | user_data_get()->security.Sensor1_open << 1 | user_data_get()->security.Sensor2_open << 2;
    net_common_send(DEVICE_ALL,NET_CMD_ABSENT_POWER,arg1,0);
    tuya_dp_232_response_absent_mode(user_data_get()->security.running);
    return true;
}


static void tuya_switch_camera(char channel){
    enum source_index ch = monitor_channel_get();
    DEBUG_LOG("ch %d -------------------------->channel %d   \n\r",ch,channel);
    if(ch != channel){
        if(ch < CCTV1){
            looup_time_set_param(CURR_OUTDOOR,Is_answer_status ? NET_COMMON_CMD_OUTDOOR_LEISURE : NET_CMD_END_CAMERA_ACK,5);
        }
        monitor_channel_set(channel);
        // audio_push_to_tuya_close();
        if(audio_stream_status_get()){
            tuya_audio_stream_open(monitor_channel_get() == CAM1 ? 0 : 1);
            audio_push_to_tuya_open();
        }
        // audio_pull_to_local_close();
        if(channel < CCTV1 && device_list_ip_get((monitor_channel_get() == CAM1)? DEVICE_OUTDOOR_1 : DEVICE_OUTDOOR_2) == NULL){
            DEBUG_LOG(" ch%d :not signel        ->DEVICE_LIST:%s\n\r",monitor_channel_get(),device_list_ip_get((monitor_channel_get() == CAM1)? DEVICE_OUTDOOR_1 : DEVICE_OUTDOOR_2));
            return;
        }
        tuya_ipc_ring_buffer_video_release_data();
        monitor_open();

        tuya_switch_channel_upload_results(channel);
    }
}

static bool is_tuya_app_busy = false;
bool tuya_event_inside_proc(unsigned int arg1, unsigned int arg2) {
    tuya_event ev = (tuya_event) arg1;
    DEBUG_LOG("\n\r\n\rev :%d----------------------->%s\n\r\n\r",ev,__func__);
    switch (ev) {
        /*切换监控*/
        case TUYA_EVENT_MONITOR_SWAP:
            tuya_ipc_ring_buffer_video_release_data();
            tuya_switch_camera(arg2);
            is_tuya_app_busy = false;
            tuya_ipc_ring_buffer_video_release_data();
            break;
            /*开锁*/
        case TUYA_EVENT_OPEN_DOOR:
            start_door_unlock();
            break;
            /* 离家模式 */
        case TUYA_EVENT_ABSENT_MODE:

            tuya_set_absent_mode(arg2);
            DEBUG_LOG("TUYA_SET_ABSENT_MODE ========================>%d\n\r",arg2);
            break;
            /*通话*/
        case TUYA_EVENT_TALK:
            camera_action = NOT;
            break;
            /*进入监控*/
        case TUYA_EVENT_MONITOR_ENTER:
            /*Don't do anything*/
        {
            monitor_enter_flag_set(MONITOR_ENTER_TUYA);
            video_decode_close();
            audio_stream_close();
        }
            break;
            /*退出监控*/
        case TUYA_EVENT_MONITOR_QUIT: {
            if(security_in_alarm_state()){
                app_layout_goto(&layout_alarm);
            }else{
                app_layout_goto(&layout_home);
            }
        }
            break;

        case TUYA_EVENT_INIT_OPEN:
            if(arg2 != user_data_get()->user_default_device &&( tuya_sdk_init_status() || user_data_get()->tuya_open)){
                DEBUG_LOG("TUYA_EVENT_INIT_OPEN ============>%d\n\r",arg2);
                user_data_get()->tuya_open = false;
                Save_config(FACTORY_CONF_FILE_PATH);
                gpio_set(37, GPIO_LEVEL_LOW);
                system("reboot");
            }
        break;
        default:
            /*Don't do anything*/
            break;
    }
    return true;
}
bool tuya_event_extern_proc(unsigned int arg1,unsigned int arg2)
{
	tuya_event ev = (tuya_event)arg1;
    DEBUG_LOG("\n\r\n\rev :%d----------------------->%s\n\r\n\r",ev,__func__);
	switch(ev)
	{
		/*切换监控*/
		case TUYA_EVENT_MONITOR_SWAP:
            tuya_ipc_ring_buffer_video_release_data();
            tuya_switch_camera(arg2);
            is_tuya_app_busy = false;
            tuya_ipc_ring_buffer_video_release_data();
		break;
		/*开锁*/
		case TUYA_EVENT_OPEN_DOOR:
            start_door_unlock();
		break;
        /* 离家模式 */
        case TUYA_EVENT_ABSENT_MODE:

            tuya_set_absent_mode(arg2);
            DEBUG_LOG("TUYA_SET_ABSENT_MODE ========================>%d\n\r",arg2);
            break;
		/*通话*/
		case TUYA_EVENT_TALK:

		break;
		/*进入监控*/
		case TUYA_EVENT_MONITOR_ENTER:
		{
			monitor_enter_flag_set(MONITOR_ENTER_TUYA);
            for(int i = DEVICE_OUTDOOR_1;i < DEVICE_TOTAL;i++){
                if(device_list_ip_get(i) != NULL){
			        monitor_channel_set(i-DEVICE_INDOOR_ID4);
                    break;
                }
            }
			app_layout_goto(&layout_door_cam);
		}
		break;
		/*退出监控*/
		case TUYA_EVENT_MONITOR_QUIT:
			/*Don't do anything*/
		break;

        case TUYA_EVENT_INIT_OPEN:
            if(arg2 != user_data_get()->user_default_device &&( tuya_sdk_init_status() || user_data_get()->tuya_open)){
                DEBUG_LOG("TUYA_EVENT_INIT_OPEN ============>%d\n\r",arg2);
                user_data_get()->tuya_open = false;
                Save_config(FACTORY_CONF_FILE_PATH);
                gpio_set(37, GPIO_LEVEL_LOW);
                system("reboot");
            }
        break;
		default:
			/*Don't do anything*/
		break;
	}
	return true;
}

static unsigned long long pre_call = 0;
static void monitor_outdoor1_inside_call(void){
    long long pts = os_get_ms();
    if(pts - pre_call < 1000 && device_list_ip_get(DEVICE_OUTDOOR_1) == NULL){
        return;
    }
    pre_call = pts;
    DEBUG_LOG("MONITOR_OUTDOOR1_INSIDE_CALL================\n\r");
    // add_absent_event(CAM1);
    absent_event_save(ABSENT_EV_FILE_PATH,&absent_event_group);   

    if((monitor_channel_get() != CAM1) /* &&  *//* (Is_answer_status == false) && *//*  get_video_revice_status() */){
        

        if(monitor_channel_get() == CAM2){
            looup_time_set_param(CURR_OUTDOOR,Is_answer_status ? NET_COMMON_CMD_OUTDOOR_LEISURE : NET_CMD_END_CAMERA_ACK,5);
        }

        monitor_channel_set(CAM1);//先发送关闭前视频源命令，再切换视频源
        monitor_enter_flag_set(monitor_enter_flag_get() == MONITOR_ENTER_TUYA ? MONITOR_ENTER_TUYA : MONITOR_ENTER_CALL);

        // if(Is_answer_status)
        //     source_res_int(false,monitor_channel_get() - 1);

        Is_answer_status = false;
        is_tuya_app_busy = false;
        if(monitor_enter_flag_get() == MONITOR_ENTER_TUYA)
        {
            tuya_ipc_ring_buffer_video_release_data();
            monitor_open();
        }
        else
        {
            camera_no_signal_display(monitor_open()/* monitor_switch(monitor_channel_get(),Is_answer_status) */);

            curr_second = user_data_get()->user_default_mode_conf.time_conf.mon_time;

            monit_interface(user_conf);


            if(is_attr_window_open)
                attr_open_button_up(NULL);                
        }

    }
    
    if(!return_disturb_status() )
        door1_call_sound_play();

    if(tuya_get_app_register_status() == E_IPC_ACTIVEATED || user_data_get()->user_default_mode_conf.user_default_snap || user_data_get()->security.running)
        monitor_doorbell(true);
}
static void monitor_outdoor1_extern_call(void)
{

    // DEBUG_LOG("----------------------------------->%s\n \r",__func__);
    // extern NET_SOCK_TYPE  curr_dev_sock;
    if(device_list_ip_get(DEVICE_OUTDOOR_1) == NULL)
        return;

    if(goto_layout == &layout_alarm || device_conflict_get(OUTDOOR_CONFLICT_GET)){
        // DEBUG_LOG("----------------------------------->%d\n \r",__LINE__);
        return;
    }

    // add_absent_event(CAM1);
    absent_event_save(ABSENT_EV_FILE_PATH,&absent_event_group);   
    // source_res_int(false,monitor_channel_get() - 1);

    monitor_channel_set(CAM1);


    monitor_enter_flag_set(MONITOR_ENTER_CALL);
    app_layout_goto(&layout_door_cam);        

    if(!return_disturb_status())
	    door1_call_sound_play();

    if(tuya_get_app_register_status() == E_IPC_ACTIVEATED || user_data_get()->user_default_mode_conf.user_default_snap || user_data_get()->security.running)
        monitor_doorbell(true);
    return;
}

static void monitor_outdoor2_inside_call(void){
    long long pts = os_get_ms();
    if(pts - pre_call < 1000  && device_list_ip_get(DEVICE_OUTDOOR_2) == NULL){
        return;
    }
    pre_call = pts;
    DEBUG_LOG("MONITOR_OUTDOOR2_INSIDE_CALL================\n\r");
    // add_absent_event(CAM2);
    absent_event_save(ABSENT_EV_FILE_PATH,&absent_event_group);   

    if((monitor_channel_get() != CAM2) /* && (Is_answer_status == false) *//* && get_video_revice_status() */){
        

        if(monitor_channel_get() == CAM1){
            looup_time_set_param(CURR_OUTDOOR,Is_answer_status ? NET_COMMON_CMD_OUTDOOR_LEISURE : NET_CMD_END_CAMERA_ACK,5);
        }
        monitor_channel_set(CAM2);//先发送关闭前视频源命令，再切换视频源
        monitor_enter_flag_set(monitor_enter_flag_get() == MONITOR_ENTER_TUYA ? MONITOR_ENTER_TUYA : MONITOR_ENTER_CALL);

        // if(Is_answer_status)
        //     source_res_int(false,monitor_channel_get() - 1);

        Is_answer_status = false;
        is_tuya_app_busy = false;
        if(monitor_enter_flag_get() == MONITOR_ENTER_TUYA)
        {
            tuya_ipc_ring_buffer_video_release_data();
            monitor_open();
        }
        else
        {
            camera_no_signal_display(monitor_open()/* monitor_switch(monitor_channel_get(),Is_answer_status) */);
            
            curr_second = user_data_get()->user_default_mode_conf.time_conf.mon_time;

            monit_interface(user_conf);

            if(is_attr_window_open)
                attr_open_button_up(NULL);    
        }

    }
    if(!return_disturb_status() )
        door2_call_sound_play();

    if(tuya_get_app_register_status() == E_IPC_ACTIVEATED || user_data_get()->user_default_mode_conf.user_default_snap || user_data_get()->security.running)
        monitor_doorbell(true);
}
void monitor_outdoor2_extern_call(void)
{
    DEBUG_LOG("----------------------------------->%s\n \r",__func__);
    
    if(device_list_ip_get(DEVICE_OUTDOOR_2) == NULL)
        return;

    if(goto_layout == &layout_alarm || device_conflict_get(OUTDOOR_CONFLICT_GET)){
        DEBUG_LOG("----------------------------------->%d\n \r",__LINE__);
        return;
    }
    
    // add_absent_event(CAM2);
    absent_event_save(ABSENT_EV_FILE_PATH,&absent_event_group);   
    // source_res_int(false,monitor_channel_get() - 1);


    DEBUG_LOG("----------------------------------->%d\n \r",__LINE__);
    monitor_channel_set(CAM2);       

    monitor_enter_flag_set(MONITOR_ENTER_CALL);
    app_layout_goto(&layout_door_cam);        
    

    if(!return_disturb_status())
	    door2_call_sound_play();

    if(tuya_get_app_register_status() == E_IPC_ACTIVEATED || user_data_get()->user_default_mode_conf.user_default_snap || user_data_get()->security.running)
        monitor_doorbell(true);
    return;
}
static bool net_cam_event_inside_proc(unsigned int arg1,unsigned int arg2,unsigned int arg3)
{
	network_event ev = (network_event)arg1;
	switch(ev)
	{

		case NETWORK_EVENT_OUTDOOR_TALK:
			if((arg2 >> 4)  != user_data_get()->user_default_device && ((arg2 & 0x0F) +1) == monitor_channel_get())
			{
                DEBUG_LOG("NETWORK_EVENT_OUTDOOR_TALK          user_default_device>>>>>%d    channel:%d \n\r\n\r",arg2 >> 4,arg2 & 0x0F);
				app_layout_goto(&layout_home);
			}
		break;

		case NETWORK_EVENT_OUTDOOR_BUYS:

            if(CAMERA_STATUS.connectable[arg2 == DEVICE_OUTDOOR_1 ? CAM1_BUY:CAM2_BUY] != NONE_STATUS)
                return true;

            CAMERA_STATUS.connectable[arg2 == DEVICE_OUTDOOR_1 ? CAM1_BUY:CAM2_BUY] = BUSY_STATUS;
            DEBUG_LOG("arg2 :%d CAMERA %d BUY       TIME : %d    LINE:%d\n\r\n\r",arg2,arg2 == DEVICE_OUTDOOR_1 ? CAM1_BUY:CAM2_BUY,arg3,__LINE__);

		break;

		case NETWORK_EVENT_OUTDOOR_LEISURE:{

		    break;            
        }

		case NETWORK_EVENT_OUTDOOR_LINK:{
            // DEBUG_LOG("NETWORK_EVENT_OUTDOOR_LINK>>>>>>>>>>>>>>>>%d\n\r",arg2);
            CAMERA_STATUS.connectable[arg2 == DEVICE_OUTDOOR_1 ? CAM1_BUY:CAM2_BUY] = IDLE_STATUS;

		    break;        
        }
		default:
			/*Don't do anything*/
		break;
	}

	return true;
}
static void monitor_record_event_func(char type,bool finish)
{

	if(type == REC_MODE_MANUAL && goto_layout == &layout_door_cam)
	{
		monitor_shoot_btn_display();
	}
    // DEBUG_LOG(">>>>>>>>>>>>>>>>>>>>>>>>>>%s   end\n\r",__func__);
}

static void camera_no_signal_display(bool is_erase){
    position pos = {{352,250},{308,62}};

    if(is_erase == true){
        gui_erase(&pos,0x00);
        return;
    }
        
    resource res = resource_get(ROM_R_4_DOOR_CAM_DOOR_OPENED_PNG);
    button no_signal;
    button_init(&no_signal,&pos,NULL);
    button_icon_init(&no_signal,&res);
    button_text_init(&no_signal,btn_str(STR_NO_SIGNAL),font_size(CAM_DOOR));
    no_signal.font_color = BLUE;
    button_display(&no_signal);
}
static void layout_door_cam_init(void)
{
    action_answer_button_init();
    action_door2_button_init();
    action_home_button_init();
    action_shoot_button_init();
#if SD_CARD
    action_recode_button_init();
#endif
    monit_attr_bg_init();
    source_button_init();
    action_hangup_button_init();
    attr_open_button_init();
    attr_bri_adjust_init();
    attr_vol_adjust_init();
    attr_outdoor_vol_adjust_init();
#ifdef Set_PQ
    set_extend_button_init();
    set_extend_refresh_init();
    set_extend_close_init();
#endif
    attr_adjust_button_list();
    door_call_event_register(monitor_outdoor1_extern_call,monitor_outdoor2_extern_call);
    tuya_event_register(tuya_event_extern_proc);
}

static void layout_door_cam_enter(void)
{
    DEBUG_LOG("start========================>%s\n\r",__func__);
    gui_background_clear();
    looup_dufault_param1_set(NET_CMD_DEVICE_BUSY);
    looup_dufault_param2_set(CAMERA_BUSY_STATE);
    network_event_regiter(net_cam_event_inside_proc);
    tuya_event_register(tuya_event_inside_proc);
    door_call_event_register(monitor_outdoor1_inside_call,monitor_outdoor2_inside_call);    
    standby_timer_close();    
    monitor_open();
    if(monitor_enter_flag_get() == MONITOR_ENTER_TUYA){
        DEBUG_LOG("MONITOR_ENTER_TUYA>>>>>>>>>>>>>>>>>>>>%s \n\r",__func__);
        return;
    }

    motion_handle_thread_open(set_lcd_brightness); 
    set_lcd_brightness(user_data_get()->SOUR_PRO[monitor_channel_get()].bri); 
    record_event_register(monitor_record_event_func);

    curr_second = user_data_get()->user_default_mode_conf.time_conf.mon_time;
#ifdef Ste_PQ
    is_pq_bg_dis = false;    
    CAM_SET[LOCAL_BRI_ADJUST_BUTTON].pos.point.x= 30;    
#endif
#if SD_CARD
    delect_sdcard(true);
#endif
    monit_interface(user_conf);
    DEBUG_LOG("end========================>%s\n\r",__func__);
}



static void layout_door_cam_timer(void)
{
    static unsigned long long pre_pts = 0;
    static unsigned long long pre_roll = 0;
    static unsigned long long pre_ms = 0;
    // static bool is_sour_icon_hide = false;
    unsigned long  long pts = os_get_ms(); 
    static bool audio_input_open_status = false;
    // static int auto_snap_count = 4;

    if((pts - pre_ms)>500)
    {
        pre_ms = pts;


        if(device_conflict_get(OUTDOOR_CONFLICT_GET)){
            app_layout_goto(&layout_home);    
            return;
        }
        if (is_tuya_cloud_connected_num() > 0 && monitor_enter_flag_get() == MONITOR_ENTER_TUYA){

        }
        else if(get_video_revice_status() &&  (device_list_ip_get(monitor_channel_get() + 3) != NULL))
        {

            DEBUG_LOG("monitor_doorbell_status()>>>>>>>>>>>>>>>>>> %d  !!!!\n\r",monitor_doorbell_status());
            if(monitor_doorbell_status())
            {
                monitor_doorbell(false);
                action_shoot_button_up(NULL);
            }
            // DEBUG_LOG(" ->DEVICE_LIST:%s\n\r\n\r", ->DEVICE_LIST[CURR_OUTDOOR]);
            if(is_camera_no_signal == true){
                is_camera_no_signal = false;
                camera_no_signal_display(true);
            }
            
        }else if((monitor_channel_get() < CCTV1) && device_list_ip_get(monitor_channel_get() + DEVICE_4_BUTTON) != NULL){
            DEBUG_LOG("CAMERA REQUEST,>>>>>>>>>>>>>>>>>> %d  !!!!\n\r",monitor_channel_get());
            net_common_send((monitor_channel_get() + DEVICE_INDOOR_ID4) ,
                                                    NET_CMD_OPEN_CAMERA_REQUEST,
                                                    (monitor_enter_flag_get() == MONITOR_ENTER_TUYA) ? 0xff : curr_second,
                                                    monitor_channel_get());
            if(get_monitor_open_flag() == false)
            {
                monitor_open();
            }
            // DEBUG_LOG("CAMERA RECOVER SIGNAL>>>>>>>>>>>>>>>>>>!!!!\n\r");

        }else if((device_list_ip_get(monitor_channel_get() + DEVICE_INDOOR_ID4) == NULL || (monitor_channel_get() > CAM2 && rtsp_stream_status_get() == -1))&&
            is_camera_no_signal == false ){
            DEBUG_LOG("CAMERA NO SIGNAL>>>>>>>>>>>>>>>>>>!!!!\n\r");
            is_camera_no_signal = true;
            // set_video_revice_status(false);
            camera_no_signal_display(false);
        }

        if((device_status_update() || get_layout_switch_flag()) && (monitor_enter_flag_get() != MONITOR_ENTER_TUYA)){
            if(source_online_num_get() != source_online_count)
            {
                source_button_using();
            }
        }

        network_cmd_send_start();
#if SD_CARD
        static bool is_sd_insert =false;
        bool flag = delect_sdcard(false);
        if(is_sd_insert != flag)
        {
            is_sd_insert = flag;
            monitor_record_video_btn_display();
            monitor_shoot_btn_display();
            if(!flag && (is_record_video_ing()== true))
            {
                action_recode_button_up(NULL);
            }
        }
#endif
        if(audio_input_open_status != get_input_open_status()){
            audio_input_open_status = get_input_open_status();
            if(audio_input_open_status){
                // DEBUG_LOG("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n\r");
                init_talk_sound(monitor_channel_get());
            }
        }
    }

    if((pts - pre_pts)>1000)
    {
        // audio_input_volume_get();
        // audio_volume_get();
        pre_pts = pts;
        if (is_tuya_cloud_connected_num() > 0 && (monitor_enter_flag_get() == MONITOR_ENTER_TUYA)) {
            if (is_tuya_app_busy == false) {
                monitor_tuya_appp_busy_display();
                button_listens_destroy();
                Is_answer_status = true;
                is_tuya_app_busy = true; 
                network_cmd_param_set(CURR_OUTDOOR,NET_CMD_ANSWER_CAMERA_REQUEST,0xff,monitor_channel_get(),5);
                if(device_list_ip_get(monitor_channel_get() + 3) != NULL)
                {
                    tuya_switch_channel_upload_results(monitor_channel_get());
                    if(monitor_channel_get() < CCTV1){
                        tuya_audio_stream_open(monitor_channel_get() == CAM1 ? 0 : 1);
                    }
                }
            }
        } 
        else 
        {

            if (is_tuya_app_busy == true) {
                is_tuya_app_busy = false;
                if(monitor_channel_get() < CCTV1)
                    tuya_audio_stream_close();
            }


            if(curr_second < 0){
                amp_enable(false);
                app_layout_goto(&layout_home);          
                return;
            }
            else{
                count_display(curr_second--,false);
            }
        }

        if(!is_tuya_app_busy)
        {
            action_hint_display(STR_TOTAL,0);
        }

    }

    if(camera_action != NOT && (monitor_enter_flag_get() != MONITOR_ENTER_TUYA))
    {
        if((pts - pre_roll)>30)
        {
            pre_roll = pts;
            action_ball_roll();
        }
    }

}

static void layout_door_cam_quit(void)
{
    DEBUG_LOG("===================>>>%s ^^^^^\n\r",__func__);

    monitor_close();

    motion_handle_thread_close();
	/*视频流退出后，会将最后一帧视频残留在背景*/
    tuya_event_register(tuya_event_extern_proc);
    door_call_event_register(monitor_outdoor1_extern_call,monitor_outdoor2_extern_call);
    network_event_regiter(NULL);
    looup_time_set_param(CURR_OUTDOOR,Is_answer_status ? NET_COMMON_CMD_OUTDOOR_LEISURE : NET_CMD_END_CAMERA_ACK,5);
    network_cmd_param_set(0,0,0,0,0);
    monitor_enter_flag_set(MONITOR_ENTER_NONE);
    standby_timer_open(standby_time,NULL);
    monitor_doorbell(false);
    looup_dufault_param1_set(NET_CMD_DEVICE_NONE);
    looup_dufault_param2_set(NET_CMD_DEVICE_NONE);
    Is_answer_status = false;
    camera_action = NOT;
    is_attr_window_open = false;
    is_tuya_app_busy = false;
    video_display_normal = false;
    is_camera_no_signal = false;
    Save_config(FACTORY_CONF_FILE_PATH); 
}
layout layout_door_cam = 
{
    .init = layout_door_cam_init,
    .enter = layout_door_cam_enter,
    .quit = layout_door_cam_quit,
    .timer = layout_door_cam_timer
};