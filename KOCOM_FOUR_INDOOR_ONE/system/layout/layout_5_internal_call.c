#include <math.h>
#include<stdio.h>
#include"ui_api.h"
#include"os_sys_api.h"
#include"rom.h"
#include"layout_common.h"
#include"layout_base.h"
#include"language.h"
#include"l_network.h"
#include"l_audio_play.h"
#include"layout_internal_button.h"
#include"network_function_api.h"
#include"audio_play_api.h"
#include"debug.h"


#define PRINTFS    1
#define UDP_INTERNAL

#define DISABLING_DEVICE_CALLS(x,y)                  { /* DEBUG_LOG("Disabling_device_Calls ------------------------> %d\n\r",__LINE__); */\
                                                                                                   Disabling_device_Calls((x),(y)); \
                                                                                                }\

static void volume_adjust_but_display(void);
#define Volume_adjust_but_display()                  { /* DEBUG_LOG("Volume_adjust_but_display ------------------------> %d\n\r",__LINE__); */\
                                                                                                   volume_adjust_but_display(); \
                                                                                                }\

static int session_number = 0;//会话序号
static int return_second = 30;
static network_device target_dev = DEVICE_TOTAL;
enum device_internal_status internal_status = IDLE_STATUS;
NET_SOCK_TYPE  curr_dev_sock = SOCK_NONE; 
//button *device_but = CALL_DEVICE[user_data_get()->user_default_device];

static void text_internal_call_display(void){
	text internal_call;
    position pos = {{177,95},{400,70}};
	text_init(&internal_call, &pos, font_size(HEADLINE));
    internal_call.align = LEFT_MIDDLE;
	language_text_display(&internal_call,text_str(STR_INTERNAL),language_get());
}

static bool call_button_ring_display(const button *Button)
{
    curr_dev_sock = SOCK_SERVER;
    //gui_erase(&Button->pos,0x00);
    ((button *)Button)->erase = true;
    resource res= resource_get(ROM_R_5_INTERNAL_CALL_CALL_RING_PNG);
    button_icon_init((button *)Button,&res);
    button_display((button *)Button);
    return true;
}

button volume_adjust_button;
static void volume_size_display(void)
{
        position pos = {{385,499},{volume_adjust_button.icon_offset.x-10,5}};
        draw_rect(&pos,WHITE);
}
static void vol_but_icon_display(void){

    resource  resC = resource_get(ROM_R_5_INTERNAL_CALL_CALL_ICON_PNG);
    resource  resT = resource_get(ROM_R_5_INTERNAL_CALL_TALK_ICON_PNG);
    position pos = {{317,483},{35,35}};
    icon vol;

    gui_erase(&pos,0x00);
    icon_init(&vol,&pos,((internal_status != TALK_STATUS) ? &resC : &resT));
    icon_display(&vol);
}
static void volume_adjust_but_display(void){
    vol_but_icon_display();
    volume_adjust_button.erase = true;

    if(internal_status != TALK_STATUS){
        volume_adjust_button.icon_offset.x = (user_data_get()->SOUR_PRO[DEV].sound.call_vol-1)*60;
    }else{
        volume_adjust_button.icon_offset.x = (user_data_get()->SOUR_PRO[DEV].sound.talk_vol-1)*60;
    }
    button_display(&volume_adjust_button);
    volume_size_display();
}
static void volume_adjust_button_up(const void *arg){
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
}

static void volume_slide_dispaly(int slide_x)
{
    if(slide_x > 680)  slide_x = 680;

    int vol_level = abs(slide_x - volume_adjust_button.pos.point.x)/60;

    if(internal_status != TALK_STATUS){
        static int prev_call_volume = 0;
        if((vol_level != prev_call_volume))
        {
            prev_call_volume = vol_level;
            volume_adjust_button.icon_offset.x = vol_level * 60;
            user_data_get()->SOUR_PRO[DEV].sound.call_vol = vol_level + 1;

            // }
            // DEBUG_LOG("user_data_get()->SOUR_PRO[DEV].sound.call_vol ===>vol_level :%d\n\r",user_data_get()->SOUR_PRO[DEV].sound.call_vol);
            volume_adjust_button.erase = true;
            button_display(&volume_adjust_button);
            volume_size_display();
        }

        motion_audio_set_volume(user_data_get()->SOUR_PRO[DEV].sound.call_vol*TONE_VOLUME_INTERVAL+TONE_MIN_VOLUME,
                                                user_data_get()->SOUR_PRO[DEV].sound.call_mel,
                                                user_data_get()->SOUR_PRO[DEV].sound.speak_sen,
                                                true);
        // prev_call_volume = abs(vol_level - prev_call_volume)>1?vol_level:prev_call_volume;        
    }else{
        static int prev_talk_volume = 0;
        if((vol_level != prev_talk_volume))
        {
            prev_talk_volume = vol_level;
            volume_adjust_button.icon_offset.x = vol_level * 60;
            user_data_get()->SOUR_PRO[DEV].sound.talk_vol = vol_level+1;

            volume_adjust_button.erase = true;
            button_display(&volume_adjust_button);
            volume_size_display();
        }
        motion_audio_set_volume(user_data_get()->SOUR_PRO[DEV].sound.talk_vol*TALK_VOLUME_INTERVAL+TALK_MIN_VOLUME_INTERVAL,0,0,false);
        // prev_talk_volume = abs(vol_level - prev_talk_volume)>1?vol_level:prev_talk_volume;        
    }

}
static void volume_adjust_button_down(const void *arg)
{
    const point * cur_x = touch_current_point();
    // DEBUG_LOG("cur_x ==================>%d\n\r",cur_x->x);
    volume_slide_dispaly(cur_x->x);
}
static void volume_adjust_button_motion(const void *arg)
{
    const point * cur_x = touch_current_point();
    volume_slide_dispaly(cur_x->x );
}
static void volume_adjust_button_init(void)
{
    position pos = {{365,473},{380,55}};
    button_init(&volume_adjust_button,&pos,volume_adjust_button_up);
    resource res =resource_get(ROM_R_5_INTERNAL_CALL_VOLUME_ADJUST_PNG);
    button_icon_init(&volume_adjust_button,&res);
    volume_adjust_button.motion = volume_adjust_button_motion;
    volume_adjust_button.down = volume_adjust_button_down;
    volume_adjust_button.icon_align = LEFT_MIDDLE;
    volume_adjust_button.png_buffer = true;
    button_tone_mute(&volume_adjust_button);
    // volume_adjust_button.bg_color =BLUE;
}

static void Disabling_device_Calls(enum call_device_button dev,bool enable);
static void device_hint_display(bool is_erase,enum btn_string_id id);

static void device_call_button_up(const void *arg)
{
    static unsigned long long prev_press_time = 0;
    unsigned long long  curr_press_time = os_get_ms();//按键半秒响应一次
    if(curr_press_time - prev_press_time > 600){
        prev_press_time = curr_press_time;
    }else{
        return;
    }
    DEBUG_LOG("----------------------------------------- : %s\n\r",__func__);

    if(Par_Button->key_enable == false)//按键功能禁用
    {
        return;
    }

    int device_call = SequenceSearch(CALL_DEVICE,Par_Button,DEVICE_CALL_ALL);   
    //gui_erase(&(Par_Button)->pos,0x00);    
    if((device_call == user_data_get()->user_default_device) || (device_call < 0) || (device_list_ip_get(device_call) == NULL)){
        return;          
    }

    if(/* internal_status == SOCK_NONE &&  */target_dev != device_call && device_busy_status_get(device_call) != NONE_BUSY_STATE)
    {
            DEBUG_LOG("-----------------------------------------DEVICE_BUSY :++++++++++++++++++ %d\n\r",device_busy_status_get(device_call));
        if(curr_dev_sock == SOCK_NONE && device_busy_status_get(device_call) == INTERCOM_BUSY_STATE){
            // target_dev = device_call;
            // internal_status = LINK_STATUS;//设备呼叫请求，避免设备同时呼叫，在等待ring回复前收到呼叫改变状态，后收到之前呼叫ring回复而导致冲突
            device_hint_display(false,STR_DEVICE_BUSY);
            return;
        }
        return;
    }

    // DEBUG_LOG("-----------------------------------------CURR_DEV_SOCK : %d\n\r",curr_dev_sock);
    switch (device_call)
    {
        // case CALL_ALL_BUTTON:
        // button_control_group(CALL_DEVICE,DEVICE1_CALL_BUTTON,CALL_ALL_BUTTON,call_button_ring_display);
        // button_display(Par_Button);
        //     break;
        case DEVICE1_CALL_BUTTON:
        case DEVICE2_CALL_BUTTON:
        case DEVICE3_CALL_BUTTON:
        case DEVICE4_CALL_BUTTON:
            if(curr_dev_sock == SOCK_NONE){
                target_dev = device_call;
                internal_status = LINK_STATUS;//设备呼叫请求，避免设备同时呼叫，在等待ring回复前收到呼叫改变状态，后收到之前呼叫ring回复而导致冲突
                network_cmd_param_set(target_dev,NET_COMMON_CMD_INTERNAL_CALL,session_number,0,4);
                #if PRINTFS
                DEBUG_LOG("[send]>>>>>>>>>2>>>>>>>>>>>NET_COMMON_CMD_INTERNAL_CALL   session_number:%d\n\r",session_number);
                #endif
                return;
            }
            else if(curr_dev_sock == SOCK_CLINET){
                network_cmd_param_set(target_dev,NET_COMMON_CMD_INTERNAL_CANCEL,session_number++,0,4);
                #if PRINTFS
                DEBUG_LOG("[send]>>>>>>>>>>>>>>>>>>>>NET_COMMON_CMD_INTERNAL_CANCEL   target_dev:%d     session_number:%d\n\r",target_dev,session_number);
                #endif
                curr_dev_sock = SOCK_NONE;        
                button_tone_open(Par_Button);
                button_refresh(Par_Button,ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG,ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG_SIZE,true);
                if(internal_status == TALK_STATUS){
                    #ifdef UDP_INTERNAL
                    internal_udp_close();
                    #endif
                    standby_timer_open(standby_time,NULL);
                }
                if(internal_status == CALL_STATUS){
                    Audio_play_stop();
                }
                internal_status = IDLE_STATUS;
                target_dev = DEVICE_TOTAL;
                DISABLING_DEVICE_CALLS(device_call,true);
            }
            else if(curr_dev_sock == SOCK_SERVER){
                if(internal_status != TALK_STATUS){
                    #if PRINTFS
                    DEBUG_LOG("[send]>>>>>>>>>>>>>>>>>>>>NET_COMMON_CMD_INTERNAL_ACK   session_number:%d\n\r",session_number);
                    #endif
                    network_cmd_param_set(target_dev,NET_COMMON_CMD_INTERNAL_ACK,session_number,0,4);
                    internal_status = TALK_STATUS;
                    return_second = 180;
                    button_refresh(Par_Button,ROM_R_5_INTERNAL_CALL_CALL_LINE_PNG,ROM_R_5_INTERNAL_CALL_CALL_LINE_PNG_SIZE,true);
                    button_tone_mute(Par_Button);
                    Audio_play_stop();
                    // count_display(0,true);
                    standby_timer_close();
                    #ifdef UDP_INTERNAL
                    internal_udp_open(device_list_ip_get(user_data_get()->user_default_device),
                                                            12000,
                                                            curr_dev_sock,
                                                            user_data_get()->SOUR_PRO[DEV].sound.talk_vol * TALK_VOLUME_INTERVAL - 80 + TALK_MIN_VOLUME_INTERVAL,
                                                            user_data_get()->SOUR_PRO[DEV].sound.speak_sen/* *2 */);
                    #endif
                    amp_enable(true);
                    device_hint_display(true,STR_TOTAL);
                }
                else{
                    internal_status = IDLE_STATUS;
                    curr_dev_sock = SOCK_NONE;        
                    network_cmd_param_set(target_dev,NET_COMMON_CMD_INTERNAL_CANCEL,session_number++,0,4);
                    target_dev = DEVICE_TOTAL;//一定要放在发送后面，不然发送取消指令目标接受不到
                    button_refresh(Par_Button,ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG,ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG_SIZE,true);              
                    button_tone_open(Par_Button);
                    #ifdef UDP_INTERNAL
                    internal_udp_close();
                    #endif
                    standby_timer_open(standby_time,NULL);
                    DISABLING_DEVICE_CALLS(device_call,true);
                    #if PRINTFS
                    DEBUG_LOG("[send]2>>>>>>>>>>>>>>>>>>>>NET_COMMON_CMD_INTERNAL_CANCEL     target_dev:%d      session_number%d\n\r",target_dev,session_number);
                    #endif
                }
                
            }
            Volume_adjust_but_display();
            count_display(0,true);
            break;
        default:
            break;
    }   
    //button_control_group(CALL_DEVICE,DEVICE1_CALL_BUTTON,DEVICE_CALL_ALL,button_disable);


}
// static void device_call_button_leave_down(const void *arg)
// {
//     if(curr_dev_sock != SOCK_NONE) return;
//     int device_call = SequenceSearch(CALL_DEVICE,Par_Button,DEVICE_CALL_ALL);   
//     if((device_list_ip_get(device_call) != NULL) && (device_call != user_data_get()->user_default_device))
//     {
//         button_refresh(Par_Button,ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG,ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG_SIZE,true);           
//     }
// }
static void device_call_button_init(button *Button,position pos,resource res,enum btn_string_id id)
{
    button_init(Button,&pos,device_call_button_up);
    button_icon_init(Button,&res);
    button_text_init(Button,btn_str(id),font_size(INTERCOM_BUTTON));
    Button->icon_align = CENTER_TOP;
    Button->font_align = CENTER_BOTTOM;
    // Button->down = device_call_button_down;
    // Button->leave_down = device_call_button_leave_down;
    // Button->short_down = device_call_button_short_down;
}

static void device_call_button_list(void)
{
    curr_dev_sock = SOCK_NONE;
    position pos = {{177,210},{148,210}};    
    resource res= resource_get(ROM_R_5_INTERNAL_CALL_NO_DEVICE_PNG);    
    return_second = 30;
//     device_call_button_init(&CALL_DEVICE[DEVICE1_CALL_BUTTON],pos,res,STR_DEVICE1);        
//     pos.point.x = 372;
//     device_call_button_init(&CALL_DEVICE[DEVICE2_CALL_BUTTON],pos,res,STR_DEVICE2);        
//     pos.point.x = 538;
//     device_call_button_init(&CALL_DEVICE[DEVICE3_CALL_BUTTON],pos,res,STR_DEVICE3);        
//     pos.point.x = 703;
//     device_call_button_init(&CALL_DEVICE[DEVICE4_CALL_BUTTON],pos,res,STR_DEVICE4);        

    for(int i = 0;i < DEVICE_CALL_ALL;i++){
        device_call_button_init(&CALL_DEVICE[DEVICE1_CALL_BUTTON + i],pos,res,STR_DEVICE1 +i);
        button_tone_open(&CALL_DEVICE[i]);
        pos.point.x += 177;
    }    
   
}

static void Disabling_device_Calls(enum call_device_button dev,bool is_enable){
    for(int i = DEVICE1_CALL_BUTTON;i<DEVICE_CALL_ALL;i++){
        if(dev != i){
            CALL_DEVICE[i].button_tone = is_enable ? KEY_1 : KEY_2;
            CALL_DEVICE[i].key_enable = is_enable;
            // if(is_enable == false){
                // if(i != user_data_get()->user_default_device)
                // {
                //     CALL_DEVICE[i].font_color = GRAY; 
                //     button_refresh(&CALL_DEVICE[i],ROM_R_5_INTERNAL_CALL_NO_DEVICE_PNG,ROM_R_5_INTERNAL_CALL_NO_DEVICE_PNG_SIZE,true); 
                // }
            //     button_disable(&CALL_DEVICE[i]);
            // }
            // else{
                // if(i != user_data_get()->user_default_device)
                // {
                //     if(device_list_ip_get(i) == NULL)
                //     {
                //         CALL_DEVICE[i].font_color = GRAY; 
                //         button_refresh(&CALL_DEVICE[i],ROM_R_5_INTERNAL_CALL_NO_DEVICE_PNG,ROM_R_5_INTERNAL_CALL_NO_DEVICE_PNG_SIZE,true); 
                //     }
                //     else
                //     {
                //         CALL_DEVICE[i].font_color = WHITE; 
                //         button_refresh(&CALL_DEVICE[i],ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG,ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG_SIZE,true);  
                //     }

                // }
            //     button_enable(&CALL_DEVICE[i]);
            // }
        }
    }
}

char hint_close_time;
static void device_hint_display(bool is_erase,enum btn_string_id id)
{
    position pos_ui = {{370,20},{308,62}};
    position pos_str = {{370,15},{308,62}};

    if(is_erase) {
        gui_erase(&pos_ui,0x00);
        return;
    }
    hint_close_time = 3;
    icon hint_ui;
    text  hint_str;
    resource res = resource_get(ROM_R_5_INTERNAL_CALL_DEVICE_BUSY_PNG);

    icon_init(&hint_ui,&pos_ui,&res);
    icon_display(&hint_ui);
    text_init(&hint_str,&pos_str,28);
    hint_str.align = CENTER_MIDDLE;
    hint_str.font_color = WHITE;
    language_text_display(&hint_str,text_str(id),language_get());
}

static bool net_internal_event_inside_proc(unsigned int arg1,unsigned int arg2,unsigned int arg3)
{
	network_event ev = (network_event)arg1;
    standby_timer_reset();
	switch(ev)
	{
		case NETWORK_EVENT_INTERNAL_TALK:{
            if(internal_status != TALK_STATUS  && session_number == arg3){
                #if PRINTFS
            	DEBUG_LOG("[receive]>>>>>>>>>>>>>>>>>>>>NETWORK_EVENT_INTERNAL_TALK     session_number:%d     curr_dev_sock:%d\n\r",arg3,curr_dev_sock);
                #endif
                network_cmd_param_set(target_dev,0,0,0,0);
                
                return_second = 180;
                internal_status = TALK_STATUS;
                curr_dev_sock = SOCK_CLINET;
                Audio_play_stop();
                
                count_display(0,true);
                button_refresh(&CALL_DEVICE[arg2],ROM_R_5_INTERNAL_CALL_CALL_LINE_PNG,ROM_R_5_INTERNAL_CALL_CALL_LINE_PNG_SIZE,true);           
                standby_timer_close();

                button_tone_mute(&CALL_DEVICE[arg2]);
                #ifdef UDP_INTERNAL
                internal_udp_open(device_list_ip_get(arg2),
                                                            12000,
                                                            curr_dev_sock,
                                                            user_data_get()->SOUR_PRO[DEV].sound.talk_vol * TALK_VOLUME_INTERVAL - 80 + TALK_MIN_VOLUME_INTERVAL,
                                                            user_data_get()->SOUR_PRO[DEV].sound.speak_sen*2);
                #endif
                amp_enable(true);
                device_hint_display(true,STR_TOTAL);
                   
            }

        }
		break;
		case NETWORK_EVENT_INTERNAL_CANCEL:{
                    #if PRINTFS
                    DEBUG_LOG("[receive]>>>>>>>>>>>>>>>>>>>>NETWORK_EVENT_INTERNAL_CANCEL     session_number:%d\n\r",arg3);
                    #endif
                if(internal_status != IDLE_STATUS && session_number == arg3){
                    network_cmd_param_set(target_dev,0,0,0,0);
                    session_number ++;
                    if(internal_status == TALK_STATUS){
                        #ifdef UDP_INTERNAL     
                        internal_udp_close();         
                        #endif 
                        standby_timer_open(standby_time,NULL);          
                    }
                    internal_status = IDLE_STATUS;
                    target_dev = DEVICE_TOTAL;
                    Audio_play_stop();
                    count_display(0,true);
                    curr_dev_sock = SOCK_NONE;        
                    button_refresh(&CALL_DEVICE[arg2],ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG,ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG_SIZE,true);             
                    button_tone_open(&CALL_DEVICE[arg2]);
                    DISABLING_DEVICE_CALLS(arg2,true);                             
                }
        }
		break;
        case NETWORK_EVENT_INTERNAL_BUSY:{
            #if PRINTFS
            DEBUG_LOG("[receive]>>>>>>>>>>>>>>>>>>>>NETWORK_EVENT_INTERNAL_BUSY     session_number:%d\n\r",arg3);
            #endif
            network_cmd_param_set(target_dev,0,0,0,0);
            target_dev = DEVICE_TOTAL;
            device_hint_display(false,STR_DEVICE_BUSY);
            internal_status = IDLE_STATUS;
        }
        break;
        case NETWORK_EVENT_INTERNAL_RING:{
            if(curr_dev_sock == SOCK_NONE && internal_status == LINK_STATUS && session_number == arg3){
                #if PRINTFS
                DEBUG_LOG("[receive]>>>>>>>>>>>>>>>>>>>>NETWORK_EVENT_INTERNAL_RING     session_number:%d\n\r",arg3);
                #endif
                network_cmd_param_set(target_dev,0,0,0,0);
                return_second = 30;                
                internal_status = CALL_STATUS;
                curr_dev_sock = SOCK_CLINET;
                DISABLING_DEVICE_CALLS(arg2,false);
                button_refresh(&CALL_DEVICE[arg2],ROM_R_5_INTERNAL_CALL_CALL_RING_PNG,ROM_R_5_INTERNAL_CALL_CALL_RING_PNG_SIZE,true);           
                device_hint_display(true,STR_TOTAL);
            }
        }
        break;

        case NETWORK_EVENT_INTERNAL_CONFLICT:
                network_cmd_param_set(target_dev,0,0,0,0);
                device_hint_display(false,STR_DEVICE_BUSY);
                internal_status = IDLE_STATUS;
        break;
		default:
			/*Don't do anything*/
		break;
	}
    Volume_adjust_but_display();
	return true;
}

static void internal_call_func(int ch,int arg)
{
    static unsigned long long prev_press_time = 0;
    unsigned long long  curr_press_time = os_get_ms();//按键半秒响应一次
    if(curr_press_time - prev_press_time > 600){
        prev_press_time = curr_press_time;
    }else{
        return;
    }
    #if PRINTFS
    DEBUG_LOG("[receive]>>>>>>>>>>>>>>>>>>>>NET_COMMON_CMD_INTERNAL_CALL      session_number:%d\n\r",arg);
    #endif

    if ( curr_dev_sock != SOCK_NONE || (internal_status != IDLE_STATUS)){
        #if PRINTFS
        DEBUG_LOG("[send]>>>>>>>>>>>>>>>>>>>>NET_COMMON_CMD_INTERNAL_BUSY     session_number:%d\n\r",session_number);
        #endif
        net_common_send(ch,NET_COMMON_CMD_INTERNAL_BUSY,session_number,NET_COMMON_CMD_INTERNAL_BUSY);
        return;
    }

    if(goto_layout == &layout_door_cam)
    {
        return;
    }

    if(device_conflict_get(INDOOR_CONFLICT_GET))
    {
        net_common_send(ch,NET_COMMON_CMD_DETECTION_CONFLICT,session_number,NET_COMMON_CMD_DETECTION_CONFLICT);
        return;
    }

    standby_timer_reset();

    if((goto_layout != &layout_internal_call)){
        app_layout_goto(&layout_internal_call);
    }

    #if PRINTFS
    DEBUG_LOG("[send]>>>>>>>>>>>>>>>>>>>>NET_COMMON_CMD_INTERNAL_RING session_number:%d\n\r",arg);
    #endif
    session_number = arg;
    target_dev = ch;
    return_second = 30;
    hint_close_time = 1;
    DISABLING_DEVICE_CALLS(ch,false);
    call_button_ring_display(&CALL_DEVICE[ch]);        
    internal_status = CALL_STATUS;
    network_cmd_param_set(target_dev,NET_COMMON_CMD_INTERNAL_RING,session_number,0,4);
	return;
}

static void device_status_res_init(void){
    for(int  i = 0;i<DEVICE_CALL_ALL;i++){
        if(i == user_data_get()->user_default_device){
            CALL_DEVICE[i].resource.id = ROM_R_5_INTERNAL_CALL_DEVICE_CALL_DEFAULT_PNG;
            CALL_DEVICE[i].resource.size = ROM_R_5_INTERNAL_CALL_DEVICE_CALL_DEFAULT_PNG_SIZE;    
            CALL_DEVICE[i].button_tone = KEY_1;
            CALL_DEVICE[i].font_color = BLUE;
            continue;
        }
        if(device_list_ip_get(i) != NULL){
            CALL_DEVICE[i].resource.id = ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG;
            CALL_DEVICE[i].resource.size = ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG_SIZE;    
            CALL_DEVICE[i].button_tone = KEY_1;
            CALL_DEVICE[i].font_color = WHITE;
        }else{
            CALL_DEVICE[i].font_color = GRAY;
        }
    }
}
static void internal_call_display(void)
{
    button_display(&goto_home_button);
    button_control_group(CALL_DEVICE,DEVICE1_CALL_BUTTON,DEVICE_CALL_ALL,button_display);
    Volume_adjust_but_display();
}

static void internal_call_enable(void)
{
    button_enable(&goto_home_button);
    // button_enable(&CALL_DEVICE[CALL_ALL_BUTTON]);
    //button_enable(&CALL_DEVICE[user_data_get()->user_default_device]);
    button_control_group(CALL_DEVICE,DEVICE1_CALL_BUTTON,DEVICE_CALL_ALL,button_enable);
    button_enable(&volume_adjust_button);
}
static void layout_internal_call_init(void)
{
    goto_home_button_init();
    device_call_button_list();
    volume_adjust_button_init();
    internal_call_event_register(internal_call_func);
}


static void layout_internal_call_enter(void)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__); 
    target_dev = DEVICE_TOTAL;
    DISABLING_DEVICE_CALLS(DEVICE_CALL_ALL,true);
    wallpaper_dispaly(CALL_BG);
    motion_handle_thread_open(NULL);
    goto_home_button_display(true);
    device_status_res_init();

    internal_call_display();
    internal_call_enable();
    text_internal_call_display();
    network_event_regiter(net_internal_event_inside_proc);
    
}

static void layout_internal_call_timer(void)
{
    static unsigned long long pre_pts = 0;
    unsigned long  long pts = os_get_ms();
    static unsigned long long time_100ms = 0;
    static int dev_status_get_interval = 5;
    static bool audio_input_open_status = false;
    static enum device_internal_status prev_internal_status = IDLE_STATUS;
    if((pts - pre_pts)> 1000)
    {
        pre_pts = pts;
    // audio_volume_get();
        if(audio_input_open_status != get_input_open_status()){
            audio_input_open_status = get_input_open_status();
            if(audio_input_open_status){
                // DEBUG_LOG("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n\r");
                init_talk_sound(DEV);             
            }
        }

        if((curr_dev_sock != SOCK_NONE))
        {
             if(return_second < 0){
                if(curr_dev_sock == SOCK_SERVER){
                    amp_enable(false);
                    app_layout_goto(&layout_home);
                }else if(curr_dev_sock == SOCK_CLINET){
                    device_call_button_up(&CALL_DEVICE[target_dev]);
                    amp_enable(false);
                }

             }
             else/*  if(curr_dev_sock == SOCK_CLINET) */
             {
                count_display(return_second--,false);
                // looup_time_set_param(NET_COMMON_CMD_INTERNAL_BUSY,0,5);
             }  
        }
        

        if(hint_close_time > 0 ){
            hint_close_time --;
            if(hint_close_time == 0){
                device_hint_display(true,STR_TOTAL);
            }
        }
    }

    if(pts - time_100ms > 100){

        time_100ms = pts;
        dev_status_get_interval--;
        network_cmd_send_start();
        // DEBUG_LOG("dev_status_get_interval ======================>%d\n\r",dev_status_get_interval);
        if (dev_status_get_interval <= 0){
            dev_status_get_interval = 5;

            if(device_status_update() || get_layout_switch_flag()){
                DEBUG_LOG("device_status_update() ============================>>>\n");
                for(int i = 0;i<DEVICE_CALL_ALL;i++){
                    if(i == user_data_get()->user_default_device){
                        continue;                      
                    }else{

                        if(device_list_ip_get(i) == NULL/*  || device_busy_status_get(i) */){
                            if(i == target_dev)
                                count_display(0,true);

                            CALL_DEVICE[i].font_color = GRAY;
                            button_refresh(&CALL_DEVICE[i],ROM_R_5_INTERNAL_CALL_NO_DEVICE_PNG,ROM_R_5_INTERNAL_CALL_NO_DEVICE_PNG_SIZE,true);     
                            CALL_DEVICE[i].button_tone = KEY_2;
                        }
                        else
                        {
                            if(curr_dev_sock == SOCK_NONE){
                                CALL_DEVICE[i].font_color = WHITE;       
                                button_refresh(&CALL_DEVICE[i],ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG,ROM_R_5_INTERNAL_CALL_DEVICE_CALL_EXIST_PNG_SIZE,true);  
                            }
                            if(CALL_DEVICE[i].key_enable)//按键功能开启按键音才能修改，避免别其他事件篡改
                            {
                                CALL_DEVICE[i].button_tone = (device_busy_status_get(i) !=  NONE_BUSY_STATE) && (target_dev != i )? KEY_2 : KEY_1;   
                                DEBUG_LOG("CALL_DEVICE[%d].button_tone() ====>>>%d   target_dev :%d\n",i,CALL_DEVICE[i].button_tone,target_dev);
                            }
                        }

                    }
                }            
            }            
        }

        if(internal_status != prev_internal_status)
        {
            prev_internal_status = internal_status;
            looup_dufault_param1_set(prev_internal_status != IDLE_STATUS ?  NET_CMD_DEVICE_BUSY : NET_CMD_DEVICE_NONE);
            looup_dufault_param2_set(prev_internal_status != IDLE_STATUS ?  INTERCOM_BUSY_STATE : NET_CMD_DEVICE_NONE);
        }
    }
    
    if((internal_status == CALL_STATUS) && (is_audio_play_ing() == false) && (!return_disturb_status())){
                            motion_audio_set_volume(user_data_get()->SOUR_PRO[DEV].sound.call_vol*TONE_VOLUME_INTERVAL+TONE_MIN_VOLUME,
                                                    user_data_get()->SOUR_PRO[DEV].sound.call_mel,
                                                    user_data_get()->SOUR_PRO[DEV].sound.speak_sen,
                                                    true);
    }
}

static void layout_internal_call_quit(void)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    motion_handle_thread_close();
    looup_dufault_param1_set(NET_CMD_DEVICE_NONE);
    looup_dufault_param2_set(NET_CMD_DEVICE_NONE);
    network_event_regiter(NULL);
    if(curr_dev_sock != SOCK_NONE){
        curr_dev_sock = SOCK_NONE;
        #if PRINTFS
        DEBUG_LOG("[send]>>>>>>>>>>>>>>>>>>>>NET_COMMON_CMD_INTERNAL_CANCEL session_number:%d\n\r",session_number);
        #endif
        net_common_send(target_dev,NET_COMMON_CMD_INTERNAL_CANCEL,session_number,NET_COMMON_CMD_INTERNAL_CANCEL);
        #ifdef UDP_INTERNAL
        internal_udp_close();      
        #endif
    }
    session_number = 0;
    internal_status = IDLE_STATUS;
}
layout layout_internal_call = 
{
    .init = layout_internal_call_init,
    .enter = layout_internal_call_enter,
    .quit = layout_internal_call_quit,
    .timer = layout_internal_call_timer
};