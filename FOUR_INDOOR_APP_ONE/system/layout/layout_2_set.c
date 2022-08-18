#include"ui_api.h"
#include"os_sys_api.h"
#include"layout_common.h"
#include"language.h"
#include"l_user_config.h"
#include"malloc.h"
#include"string.h"
#include"stdlib.h"
#include"audio_play_api.h"
#include"l_audio_play.h"
#include"network_function_api.h"
#include"l_absent_ev.h"
#include"l_network.h"
#include"l_cctv_conf.h"
#include"l_hardware_control.h"
#include"tcp_upgrade.h"
#include"tuya_sdk_api.h"
#include"layout_keyborad_button.h"
#include"debug.h"

#define MAINFUNC
#define MAIN_SET_PAGE
#define APP_VERSION              "0.7"

#define  MAIN_VERSION 0
#define  SUB_VERSION  2

static bool Pop_Operation_Tips = false;     
static bool Pop_Confirm_Tips = false;
static enum btn_string_id Operation_Tips = STR_TOTAL;     

#ifdef Set_Connect 
static bool is_open_wifi = false;
#endif
#ifdef Set_Other
static bool is_old_hide = true;
static bool is_new_hide = true;
static bool is_wifi_pass_hide = true;
static button *prev_lay= NULL;
#endif
static int but_press_time = 0;
static button *curr_main_but = &MAIN_SET[GENERAL_BUTTON];//当前主要功能设置-按键指针
static button *prev_device = NULL;
static button *prev_language = NULL;//当前语言
static button *prev_time_date= &SYSTEM_TIME[DATA_BUTTON];
static int cur_sound_sour = SOURCE_CAM1;
static CCTV_INFO *cur_cctv = NULL;

static button *backup_button_group[BUTTON_MAX];//被销毁之前的按键集合
static int backup_button_total = 0;//被销毁的总数

static void confirmation_window_display(int line_up,int line_down,int text_high,enum btn_string_id id,bool display_icon){
    text issue;
    icon reset;
    position pos = {{430,172},{450,45}};
    position pos_line = {{359,242},{600,1}};
    position pos_icon = {{589,251},{140,140}};
    position pos_erase = {{359,172},{600,250}};
    resource res_icon = resource_get(ROM_R_2_SET_WAITING_PNG);
    if(display_icon){
        gui_erase(&pos_erase,0x00);
        icon_init(&reset,&pos_icon,&res_icon);
        icon_display(&reset);
    }

    pos_line.point.y = line_up;
    draw_rect(&pos_line,0xff424542);
    pos_line.point.y = line_down;
    draw_rect(&pos_line,0xff424542);

    pos.point.y = text_high;
    text_init(&issue,&pos,font_size(SET_RESET_OPTION));
    issue.align = CENTER_TOP;
    // issue.bg_color = BLACK;
    language_text_display(&issue, text_str(id),language_get());

}

#ifdef Set_Other
/* CCTV信息-显示 
    id：CCTV的字符文本(在这是用作区分CCTV1和CCTV2)
*/
static void cctv_brand_display(const CCTV_INFO *cctv_type,bool is_erase){
    position pos = {{705,174},{200,36}};    
    text cctv;
    text_init(&cctv,&pos,font_size(SET_OPTION_INFO));
    cctv.font_color = WHITE;
    cctv.align = RIGHT_MIDDLE;
    cctv.erase = is_erase ? true : false;
    if(cctv_type->CCTV_BRAND == CCTV_HIK){
        text_display(&cctv,"HIK");    
    }else{
        text_display(&cctv,"DAH"); 
    }
}

static void cctv_info_text_init(char *cctv_info,position *pos){
        text info;
        text_init(&info,pos,font_size(SET_OPTION_INFO));
        pos->point.y += 60;
        info.font_color = WHITE;
        // info.bg_color = BLUE;
        info.align = RIGHT_MIDDLE;
        text_display(&info,cctv_info);        
}

#if 0
static void system_info_text_init(void){

        extern bool tuya_uuid_and_key_get(char* uuid,char* key);
        text sys_ver;
        text cam1_ver;
        text cam2_ver;
        
        text sys_ip;
        // text cam1_ip;
        // text cam2_ip;

        // text tuya_uuid;
        // text tuya_key;

        position pos = {{345,250},{610,40}};   
        char sys_v[32] = {0}; 
        char ip[32] = {0}; 
        char cam1[32] = {0}; 
        char cam2[32] = {0};
        // char uuid[64] = {0}; 
        // char uuid_ptr[64] = {0};
        // char key[64] = {0};
        // char key_ptr[64] = {0};

        text_init(&sys_ver,&pos,font_size(SET_OPTION_INFO));
        pos.point.y = 300;
        text_init(&cam1_ver,&pos,font_size(SET_OPTION_INFO));
        pos.point.y = 350;
        text_init(&cam2_ver,&pos,font_size(SET_OPTION_INFO));
        pos.point.y = 400;
        text_init(&sys_ip,&pos,font_size(SET_OPTION_INFO));
        // pos.point.y = 450;
        // text_init(&tuya_uuid,&pos,font_size(SET_OPTION_INFO));
        // pos.point.y = 500;
        // text_init(&tuya_key,&pos,font_size(SET_OPTION_INFO));

        sys_ver.align = cam1_ver.align = cam2_ver.align = sys_ip.align /* = tuya_uuid.align = tuya_key.align */ = LEFT_MIDDLE;
        sys_ver.font_color = cam1_ver.font_color = cam2_ver.font_color = sys_ip.font_color = /* tuya_uuid.font_color = tuya_key.font_color = */ WHITE;

        // sprintf(sys_v,"System version: %s",APP_VERSION);
        sprintf(sys_v,"%s %s",/* APP_VERSION */__DATE__,__TIME__);  
        user_data_get()->SOUR_PRO[CAM1].version = outdoor_info_get(0,OUTDOOR_VERSION);
        user_data_get()->SOUR_PRO[CAM2].version = outdoor_info_get(1,OUTDOOR_VERSION);

        if(device_list_ip_get(DEVICE_OUTDOOR_1) == NULL){
            sprintf(cam1 ,"Outdoor1 version:  No Device");
        }else{
            sprintf(cam1 ,"Outdoor1 version: %d.%d",user_data_get()->SOUR_PRO[CAM1].version/10 , user_data_get()->SOUR_PRO[CAM1].version%10);
        }
        if(device_list_ip_get(DEVICE_OUTDOOR_2) == NULL){
            sprintf(cam2 ,"Outdoor2 version:  No Device");
        }else{
            // DEBUG_LOG("cearm 2 version : %d\n\r",user_data_get()->SOUR_PRO[CAM2].version);
            sprintf(cam2 ,"Outdoor2 version: %d.%d",user_data_get()->SOUR_PRO[CAM2].version/10 , user_data_get()->SOUR_PRO[CAM2].version%10);
        }

        sprintf(ip ,"IP address: %s",get_device_ip() == NULL ? "No Network" : get_device_ip());

        // tuya_uuid_and_key_get(uuid_ptr,key_ptr);

        // sprintf(uuid ,"Tuya uuid: %s",uuid_ptr);
        // sprintf(key ,"Tuya key: %s",key_ptr);
        text_display(&sys_ver,sys_v);        
        text_display(&cam1_ver,cam1);   
        text_display(&cam2_ver,cam2);   
        text_display(&sys_ip,ip);   
        // text_display(&tuya_uuid,uuid);   
        // text_display(&tuya_key,key);   
}
#else

static void system_info_display(void) {
    position p_item = {{383, 195},
                       {540, 70}};
    char tmp_str[64] = {0};

    text info;
    text_init(&info, &p_item, font_size(SET_OPTION_INFO));
    const char *ver[language_total] = {"System Information","시스템 정보","Información del sistema","معلومات النظام ","Thông tin hệ thống","Системная информация"};
    sprintf(tmp_str, "%s :  %d.%d", ver[language_get()], MAIN_VERSION, SUB_VERSION);
    //info.font_color = 0xFF727572;

    if(user_data_get()->user_default_language == ARABIC){
        info.align = RIGHT_MIDDLE;
    }else{
        info.align = LEFT_MIDDLE;
    }
    info.erase = true;
    language_text_display(&info, tmp_str,language_get());

    p_item.point.y += 70;
    text_init(&info, &p_item, font_size(SET_OPTION_INFO));
    const char *id[language_total] = {"Device ID","디바이스 ID","Identificación del dispositivo","معرف الجهاز ","ID Thiết bị","ID устройства"};
    sprintf(tmp_str, "%s:  %d", id[language_get()], user_data_get()->user_default_device + 1);
    //info.font_color = 0xFF727572;
    if(user_data_get()->user_default_language == ARABIC){
        info.align = RIGHT_MIDDLE;
    }else{
        info.align = LEFT_MIDDLE;
    }
    info.erase = true;
    language_text_display(&info, tmp_str,language_get());


    p_item.point.y += 70;
    text_init(&info, &p_item, font_size(SET_OPTION_INFO));
    const char *ip[2][language_total] = {{"IP Address","네트워크 IP","IP de red","رقم بروتوكل الشبكة ","IP mạng","Сетевой IP"},
                                                                                {"No Network","인터넷 없음","Sin red","لا توجد شبكة","Không có mạng","Нет сети"} };
    sprintf(tmp_str, "%s: %s", ip[0][language_get()],get_device_ip() == NULL ? ip[1][language_get()] : get_device_ip());
    //info.font_color = 0xFF727572;
    if(user_data_get()->user_default_language == ARABIC){
        info.align = RIGHT_MIDDLE;
    }else{
        info.align = LEFT_MIDDLE;
    }
    info.erase = true;
    language_text_display(&info, tmp_str,language_get());



    p_item.point.y += 70;
    text_init(&info, &p_item, font_size(SET_OPTION_INFO));
    const char *compile[language_total] = {"Compile Time","컴파일 시간","Construir","إنشاء ","Tao","Компиляция"};
    sprintf(tmp_str, "%s:  %s %s", compile[language_get()], __DATE__, __TIME__);
    //info.font_color = 0xFF727572;
    if(user_data_get()->user_default_language == ARABIC){
        info.align = RIGHT_MIDDLE;
    }else{
        info.align = LEFT_MIDDLE;
    }
    info.erase = true;
    language_text_display(&info, tmp_str,language_get());

    p_item.point.y += 70;
    text_init(&info, &p_item, font_size(SET_OPTION_INFO));
    char serial_str[16] = {0};

    extern bool tuya_serial_number_get(char* serial);
    tuya_serial_number_get(serial_str);
    const char *sn[language_total] = {"Serial Number","일련 번호","Número de serie","رقم سري","Số seri","Серийный номер"};
    sprintf(tmp_str, "%s: %s", sn[language_get()], serial_str);
    //info.font_color = 0xFF727572;
    if(user_data_get()->user_default_language == ARABIC){
        info.align = RIGHT_MIDDLE;
    }else{
        info.align = LEFT_MIDDLE;
    }
    info.erase = true;
    language_text_display(&info, tmp_str,language_get());
}
#endif
static void cctv_info_display(CCTV_INFO *cctv,bool is_erase)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    position pos = {{510,232},{400,35}};    
        cctv_brand_display(cctv,is_erase);
        if(cctv->ip != NULL)
            cctv_info_text_init(cctv->ip,&pos);
        if(cctv->account != NULL)
            cctv_info_text_init(cctv->account,&pos);
        if(cctv->password != NULL)
            cctv_info_text_init(cctv->password,&pos);

}
/* 输入文本-副标题-显示
    id：标题名字符索引
    id2： 标题名字符索引*/
static void input_subtitle_diaplay(enum btn_string_id id,enum btn_string_id id2)
{
    int str = id;
    position pos = {{322,240},{190,40}};    
    for(int x = 0;x<2;x++)
    {
        text text;
        text_init(&text,&pos, language_get() == language_russian || language_get() == language_spanish ? font_size(SET_OPTION_INFO) : font_size(SET_OPTION_INFO));
        text.align = RIGHT_MIDDLE;
        language_text_display(&text,text_str(str),language_get());   
        str = id2;
        pos.point.y = 323;
    }
}
/* 输入窗口-主标题-显示
    id：标题名字符索引*/
static void window_headline_diaplay(enum btn_string_id id)
{
    text headline;
    position pos = {{460,116},{400,45}};
    text_init(&headline,&pos,font_size(SET_OPTION_INFO));
    // headline.bg_color = BLACK;
    language_text_display(&headline,text_str(id),language_get());
}
/* 提示窗口- -显示
    id：提示内容字符索引*/
static void hint_window_diaplay(enum btn_string_id id1,enum btn_string_id id2,bool display)
{
    icon window;
    position pos = {{0,0},{1024,600}};
    position pos_icon = {{224,124},{576,352}};

    if(display == false)
    {
        gui_erase(&pos,0x00);
        return;
    }
    
    draw_rect(&pos,VERY_TRANSPARENT);

    resource res = resource_get(ROM_R_2_SET_HINT_WINDOW_PNG);
    icon_init(&window,&pos_icon,&res);
    icon_display(&window);

    text hint;
    position pos_text = {{254,155},{503,45}};
    text_init(&hint,&pos_text,font_size(SET_OPTION_INFO));
    language_text_display(&hint,text_str(STR_NOTICE),language_get());

    text hint_text;
    position text_pos = {{254,180},{503,148}};
    text_init(&hint_text,&text_pos,language_get() == language_russian ||  language_get() == language_spanish ? font_size(SET_OPTION_INFO) - 2 :  font_size(SET_OPTION_INFO));
    if(id1 == STR_TUYA_OPEN_HINT1 && language_get() == language_russian)
    {
        text_display(&hint_text,"Поддерживает только одно устройство");
        hint_text.pos.point.y += 50;
        text_display(&hint_text,"и ссылку на приложение");
        hint_text.pos.point.y += 50;
        text_display(&hint_text,"Имеются повторяющиеся устройства или подключения.");
    }
    else
    {
        language_text_display(&hint_text,text_str(id1),language_get());         
        hint_text.pos.point.y += 60;
        language_text_display(&hint_text,text_str(id2),language_get());      
    }

    text hint_ack;
    position pos_ack = {{225,396},{575,80}};
    text_init(&hint_ack,&pos_ack,font_size(SET_OPTION_INFO));
    hint_ack.align = CENTER_MIDDLE;
    language_text_display(&hint_ack,text_str(STR_QUIT),language_get());
}
#endif
#ifdef Set_Connect 
static void manually_text_display(void)
{
    text manually;
    position text_pos = {{357,220},{172,34}};
    text_init(&manually,&text_pos,font_size(SET_OPTION_INFO));   
    manually.align = LEFT_MIDDLE;
    endif(&manually,(STR_MANUALLY)); 
}
#endif
/* 若当前月份的日期超出范围，即纠正 */
static void time_to_correct(struct ak_date *id,int *mon)
{
        if((id->day > (mon[id->month]-1)))
        {
            if(is_leap_feb(*id))
                id->day = 28;
            else
                id->day = mon[id->month]-1;            
        }
}

/* 复位分割线-显示 */
static void reset_cut_off_display(void)
{
    position pos_w = {{73,510},{200,2}};
    draw_rect(&pos_w,RED);
}
/* 擦除-副设置窗口 */
static void erase_bg(void)
{
    position pos = {{325,83},{654,486}};
    gui_erase(&pos,0x00);
}
/* 加减类型-按键使能
    < 、> 、+ 、 -   均为加减类型按键 */
static bool time_corrected = false;
static void add_sub_key_button_using(void)
{
    if(curr_main_but == &MAIN_SET[GENERAL_BUTTON])//如果当前主要设置功能为常规设置
    {
        if(prev_time_date == &SYSTEM_TIME[DATA_BUTTON])//若当前设置的是日期
        {
            button_control_group(GENERAL_ADJUST,UP_D_BUTTON,UP_H_BUTTON,button_start_using);//使能日期设置按键
            button_control_group(GENERAL_ADJUST,UP_H_BUTTON,GENERAL_ADJUST_TOTAL,button_disable);//屏蔽时分设置按键
        }
        else if(prev_time_date == &SYSTEM_TIME[TIME_BUTTON])//若当前设置的是时分
        {
            button_control_group(GENERAL_ADJUST,UP_H_BUTTON,GENERAL_ADJUST_TOTAL,button_start_using);     
            button_control_group(GENERAL_ADJUST,UP_D_BUTTON,UP_H_BUTTON,button_disable);
        }
    }
    else if(curr_main_but == &MAIN_SET[SOUND_BUTTON])
    {
        button_control_group(SOUND_ADJUST,CALL_VOL_SUB,SOUND_ADJUST_TOTAL,button_start_using);     
    }
    else if(curr_main_but == &MAIN_SET[MODE_BUTTON])
    {
        DEBUG_LOG("1------------->>>MODE_BUTTON \n\r");
        button_control_group(MODE_ADJUST,BRIGHTNESS_SUB,MODE_ADJUST_TOTAL,button_start_using);     
    }
}
#ifdef MAIN_SET_PAGE
static void set_button_up(const void *arg);
#endif
#ifdef Set_Other
static void other_button_up(const void *arg);
/* 返回键回调函数 */
static void return_button_up(const void *arg)
{ 
    erase_bg();    
    int return_but = SequenceSearch(RETURN_BUTTON, (Par_Button),RESET_TOTAL);
    switch (return_but)
    {
    case PASSWORD_RETURN:
        free_pointer(&input_buffer[ALARM_OLD_PASS_BOX]);//退出输入密码窗口即释放密码输入buffer
        free_pointer(&input_buffer[ALARM_NEW_PASS_BOX]);
        set_button_up(curr_main_but);   
        break;
    case DEVICE_RETURN:
        set_button_up(curr_main_but);   
        break;
    case UPGRADE_RETURN:
        button_control_group(OTHER_UPGRADE,UPGRADE_OUTDOOR1,OTHER_UPGRADE_TOTAL,button_disable);
        set_button_up(curr_main_but);   
        break;
    case CCTV_RETURN:
        button_control_group(OTHER_SET,BRAND_SET,OTHER_SET_TOTAL,button_disable);
        button_disable(&RETURN_BUTTON[CCTV_RETURN]);    
        other_button_up(&OTHER_SET[DEVICE_SET]);
        break;
#ifdef Set_Connect 
    case CONNECT_RETURN:
        button_disable(&RETURN_BUTTON[CONNECT_RETURN]);
        set_button_up(prev_lay);
        break;
#endif
    default:
        break;
    }
}
static void return_button_down(const void *arg)
{
    button_refresh(Par_Button,ROM_R_5_INTERNAL_CALL_RETURN_SELECT_PNG,ROM_R_5_INTERNAL_CALL_RETURN_SELECT_PNG_SIZE,true);
}
static void return_button_leave_down(const void *arg)
{
    button_refresh(Par_Button,ROM_R_5_INTERNAL_CALL_RETURN_PNG,ROM_R_5_INTERNAL_CALL_RETURN_PNG,true);
}
/* 返回按键-定义 */
static void return_button_init(button *Button,position pos)
{
    resource res = resource_get(ROM_R_5_INTERNAL_CALL_RETURN_PNG);
    button_init(Button,&pos,return_button_up);
    button_icon_init(Button,&res);
    Button->down = return_button_down;
    Button->leave_down = return_button_leave_down;
}
 /* 返回按键-集体定义 */
static void return_button_list(void)
{
    position pos = {{326,110},{70,50}};
    return_button_init(&RETURN_BUTTON[PASSWORD_RETURN],pos);

    return_button_init(&RETURN_BUTTON[DEVICE_RETURN],pos);

    return_button_init(&RETURN_BUTTON[UPGRADE_RETURN],pos);

    return_button_init(&RETURN_BUTTON[CCTV_RETURN],pos);
#ifdef Set_Connect 
    return_button_init(&RETURN_BUTTON[CONNECT_RETURN],pos);
#endif
}

static void input_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    curr_input_box = SequenceSearch(INPUT_BUT, (Par_Button),INPUT_TYPE_TOTAL);
    app_layout_goto(&layout_key_board);//进入键盘界面
}
/* 输入窗口-定义 */
static void input_button_init(button *Button,position pos)
 {
    resource  res = resource_get(ROM_R_2_SET_INPUT_BOX_PNG);
    button_init(Button,&pos,input_button_up);
    button_icon_init(Button,&res);
 }
 /* 输入窗口-集体定义 */
static void input_button_list(void)
{
    position pos = {{520,235},{309,48}};
    input_button_init(&INPUT_BUT[OTHER_OLD_PASSWORD],pos);
    input_button_init(&INPUT_BUT[CONNECT_WIFI_NAME],pos);
    pos.point.y = 318;
    input_button_init(&INPUT_BUT[OTHER_NEW_PASSWORD],pos);
    input_button_init(&INPUT_BUT[CONNECT_WIFI_PASSWORD],pos);
}

static void password_display(char **p);
static void input_show_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int input_type = SequenceSearch(INPUT_SHOW_BUT, (Par_Button),INPUT_SHOW_TOTAL);
    bool *is_hide = NULL;
    int str_buffer;
    switch (input_type)
    {
        case OTHER_OLD_PASSW:
                is_hide = &is_old_hide;
                str_buffer = ALARM_OLD_PASS_BOX;             
            break;
        case OTHER_NEW_PASSW:
                is_hide = &is_new_hide;
                str_buffer = ALARM_NEW_PASS_BOX;           
            break;
        case CONNECT_WIFI_PASSW:
                is_hide = &is_wifi_pass_hide;
                str_buffer = CONNECT_WIFI_PASSW_BOX;
            break;  
        default:
            break;
    }
    // Par_Button->bg_color = BLACK;
    if(!(*is_hide)){
        *is_hide = true;
        button_refresh((Par_Button),ROM_R_2_SET_INPUT_HIDE_PNG,ROM_R_2_SET_INPUT_HIDE_PNG_SIZE,true);
    }
    else{
        *is_hide = false;
        button_refresh((Par_Button),ROM_R_2_SET_INPUT_SHOW_PNG,ROM_R_2_SET_INPUT_SHOW_PNG_SIZE,true);
    }       
    password_display(&input_buffer[str_buffer]); 

}
/* 输入文本-可视按键-初始化 */
static void input_show_button_init(button *Button,position pos)
 {
    resource res = resource_get(ROM_R_2_SET_INPUT_HIDE_PNG);
    button_init(Button,&pos,input_show_button_up);
    button_icon_init(Button,&res);
    Button->bg_color = BLACK;
 }
static void input_show_button_list(void)
{
    position pos = {{829,235},{48,48}};
    input_show_button_init(&INPUT_SHOW_BUT[OTHER_OLD_PASSW],pos);
    pos.point.y = 318;
    input_show_button_init(&INPUT_SHOW_BUT[OTHER_NEW_PASSW],pos);
    input_show_button_init(&INPUT_SHOW_BUT[CONNECT_WIFI_PASSW],pos);
}
#endif
/* 开关类型-按键-操作
    flag：按键状态标志
    arg：按键 */
static void switch_on_off(bool *flag,button * arg)
{
    if(*flag)
    {
        *flag = false;
       //DEBUG_LOG("--------------->>>ALWAYS_OFF \n\r");
        button_refresh(arg,ROM_R_2_SET_SWITCH_OFF_PNG,ROM_R_2_SET_SWITCH_OFF_PNG_SIZE,true);
    }
    else
    {
        *flag = true;
        //DEBUG_LOG("--------------->>>ALWAYS_NO \n\r");
        button_refresh(arg,ROM_R_2_SET_SWITCH_ON_PNG,ROM_R_2_SET_SWITCH_ON_PNG_SIZE,true);
    }
}

static void switch_button_up(const void *arg)
 {
    int adjust_but = SequenceSearch(SWITCH_BUT, (Par_Button),SWITCH_TOTAL);

    switch (adjust_but)
    {
    case ALWAYS_NO:
        switch_on_off(&user_data_get()->user_default_mode_conf.user_default_always,Par_Button);
        break;
     case AUTO_IMAGE:
        DEBUG_LOG("--------------->>>AUTO_IMAGE \n\r");
        switch_on_off(&user_data_get()->user_default_mode_conf.user_default_snap,Par_Button);
        break;

#if SD_CARD
    case AUTO_RECODE:
        DEBUG_LOG("--------------->>>AUTO_RECODE \n\r");
        switch_on_off(&user_data_get()->user_default_mode_conf.user_default_record,Par_Button);
        break;
#endif
#ifdef Set_Connect 
    case WIFI_POWER:
        DEBUG_LOG("--------------->>>WIFI_POWER \n\r");
        switch_on_off(&is_open_wifi,Par_Button);
        if(is_open_wifi)
        {
            button_start_using(&manually_button);
            manually_text_display();
        }
        else
        {
            position pos = {{352,210},{610,310}};
            gui_erase(&pos,0x00);
            button_disable(&manually_button);
        }
        break;
#endif
    default:
        break;
    }
 }
 /* 开关类型-按键-初始化 */
static void switch_button_init(button *Button,position pos,resource res)
 {
    button_init(Button,&pos,switch_button_up);
    button_icon_init(Button,&res);
 }

 static void confirm_headline_pop(void)
 {
        static position pos = {{182, 100}, {660, 66}};
        text item_str;
        const char *str0[language_total] = {"WARNING","※ 주의","ADVERTENCIA","تحذير","CẢNH BÁO","ПРЕДУПРЕЖДЕНИЕ"};
        text_init(&item_str, &pos, font_size(SET_OPTION_INFO));
        language_text_display(&item_str, str0[language_get()],language_get());
 }
#ifdef Set_General

/* TUYA-按键-定义 */
static void general_tuya_resource_display(void){
    extern bool tuya_sdk_init_status();
    resource res_black = resource_get(ROM_R_2_SET_TUYA_APP_BLACK_PNG);
    resource res_blue = resource_get(ROM_R_2_SET_TUYA_APP_BLUE_PNG);
    resource res_red = resource_get(ROM_R_2_SET_TUYA_APP_RED_PNG);
    DEBUG_LOG("-------------------------->%s  %d   %d  \n\r",__func__,tuya_sdk_init_status(),user_data_get()->tuya_open);
    button_icon_init(&tuya_app_button,(tuya_open_device_get() != DEVICE_ALL && tuya_open_device_get() != user_data_get()->user_default_device) ? &res_red : (tuya_sdk_init_status() || user_data_get()->tuya_open ? &res_blue : &res_black));
    button_display(&tuya_app_button);
}

static void general_tuya_button_up(const void *arg){
    DEBUG_LOG("-------------------------->%s\n\r",__func__);
    extern bool tuya_sdk_init_status();
    Pop_Confirm_Tips = true;
    if(tuya_sdk_init_status()){
        confirm_interface(STR_TUYA_CLOSE_HINT,STR_TOTAL,false,VERY_TRANSPARENT,font_size(SET_OPTION_INFO));
        confirm_headline_pop();
        disable_prev_lay_button(backup_button_group,&backup_button_total);
        button_enable(&goto_home_button);
        button_start_using(&OPTION_ACK[TUYA_CTRL_ACK_BUTTON]);
        button_start_using(&OPTION_ACK[TUYA_CTRL_NO_BUTTON]);
    }
    else if(tuya_open_device_get() == DEVICE_ALL)
    {
        if(tuya_wifi_sdk_init(IPC_APP_PID,NULL,NULL))
        {
            tuya_open_device_set(user_data_get()->user_default_device);
            user_data_get()->tuya_open  = true;
            Save_config(FACTORY_CONF_FILE_PATH);
            general_tuya_resource_display();
        }
        else
        {
            // hint_window_diaplay(STR_TUYA_OPEN_HINT1,STR_TUYA_OPEN_HINT2,true);
            // disable_prev_lay_button(backup_button_group,&backup_button_total);
            // button_enable(&OPTION_ACK[NET_TIME_ACK_BUTTON]);
        }

        // confirm_interface(STR_TUYA_OPEN_HINT1,STR_TUYA_OPEN_HINT2,false,VERY_TRANSPARENT,font_size(SET_OPTION_INFO));
        // confirm_headline_pop();
    }
    else
    {
        // confirm_interface(STR_TUYA_OPEN_HINT1,STR_TUYA_OPEN_HINT2,false,VERY_TRANSPARENT,font_size(SET_OPTION_INFO));
        // confirm_headline_pop();
        hint_window_diaplay(STR_TUYA_OPEN_HINT1,STR_TUYA_OPEN_HINT2,true);
        disable_prev_lay_button(backup_button_group,&backup_button_total);
        button_enable(&OPTION_ACK[NET_TIME_ACK_BUTTON]);
    }
}
static void general_tuya_button_init(void)
{
    position pos = {{732,112},{120,100}};
    resource res = resource_get(ROM_R_2_SET_TUYA_APP_BLACK_PNG);
    button_init(&tuya_app_button,&pos,general_tuya_button_up);
    button_icon_init(&tuya_app_button,&res);
    button_text_init(&tuya_app_button,btn_str(STR_PHONE),font_size(SET_OPTION_INFO));
    tuya_app_button.icon_align = LEFT_BOTTOM;
    tuya_app_button.font_color = WHITE;
    tuya_app_button.font_align = LEFT_TOP;
}

static void general_device_id_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    if(prev_device != (Par_Button) && Par_Button->font_color == WHITE)
    {
        button_refresh(prev_device,ROM_R_2_SET_DEVICE_ID_PNG,ROM_R_2_SET_DEVICE_ID_PNG_SIZE,true);
        button_refresh(Par_Button,ROM_R_2_SET_DEVICE_ID_SELECT_PNG,ROM_R_2_SET_DEVICE_ID_SELECT_PNG_SIZE,true);
        prev_device = (Par_Button);
        user_data_get()->user_default_device = SequenceSearch(DEVICE, prev_device,DEVICE_BUTTON_TOTAL);
        network_oneself_device_set(user_data_get()->user_default_device);
        if(user_data_get()->tuya_open == true)
        {
            tuya_open_device_set(user_data_get()->user_default_device);
        }
    }
}
/* 设备切换-按键-初始化 */
static void general_device_id_button_init(button *Button,position pos,resource res,enum btn_string_id id)
{
    button_init(Button,&pos,general_device_id_button_up);
    button_icon_init(Button,&res);
    button_text_init(Button,btn_str(id),font_size(SET_OPTION_INFO));
    Button->font_color = WHITE;
    Button->font_align = CENTER_MIDDLE;
    Button->def_language = language_english;
    Button->png_buffer = true;
}
static void general_device_id_button_list(void)
{
    position pos = {{371,153},{70,60}};
    resource res = resource_get(ROM_R_2_SET_DEVICE_ID_PNG);
    for(int x = 0;x < DEVICE_BUTTON_TOTAL;x++){
    general_device_id_button_init(&DEVICE[DEVICE_1_BUTTON+x],pos,res,STR_1+x);
    pos.point.x = pos.point.x+71;        
    }
}
/* 设置主设备窗口-副标题-显示 */
static void general_device_id_subtitle_display(void)
{
    text Device_ID;
    position pos = {{371,112},{360,42}};
    text_init(&Device_ID,&pos,font_size(SET_OPTION_INFO));
    Device_ID.font_color = WHITE;
    Device_ID.align = LEFT_MIDDLE;
    language_text_display(&Device_ID,text_str(STR_DEVICE_ID),language_get());
    prev_device->resource.id = ROM_R_2_SET_DEVICE_ID_SELECT_PNG;
    prev_device->resource.size = ROM_R_2_SET_DEVICE_ID_SELECT_PNG_SIZE;
    for(int i = 0;i < DEVICE_BUTTON_TOTAL;i++){
        if(i == user_data_get()->user_default_device) {
            button_start_using(&DEVICE[i]);
            continue;
        }
        if(device_list_ip_get(i) == NULL){
            DEVICE[i].font_color = WHITE;
            DEVICE[i].button_tone = KEY_1;
        }
        else{
            DEVICE[i].font_color = BLUE;
            DEVICE[i].button_tone = KEY_2;
        } 
        button_start_using(&DEVICE[i]);
    }
}

static void general_language_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    if(prev_language != (Par_Button) /* && Par_Button != &LANGUAGE[LANGUAGE_R_BUTTON] */)
    {
        button_refresh(prev_language,ROM_R_2_SET_LANGUAGE_PNG,ROM_R_2_SET_LANGUAGE_PNG_SIZE,true);
        button_refresh(Par_Button,ROM_R_2_SET_LANGUAGE_SELECT_PNG,ROM_R_2_SET_LANGUAGE_SELECT_PNG_SIZE,true);
        prev_language = (Par_Button);
        user_data_get()->user_default_language = SequenceSearch(LANGUAGE, (Par_Button),LANGUAGE_BUTTON_TOTAL);
        language_set(user_data_get()->user_default_language);
        app_layout_goto(&layout_set);
        // prev_layout = goto_layout;

        // goto_layout = layout_set;
        // os_layout_goto(&goto_layout);
    } 
}
/* 语言选择-按键-初始化 */
static void general_language_button_init(button *Button,position pos,resource res,enum btn_string_id id,enum language language)
{
    button_init(Button,&pos,general_language_button_up);
    button_icon_init(Button,&res);
    if(id != STR_TOTAL)
        button_text_init(Button,btn_str(id),/* font_size(SET_OPTION_INFO) */22);

    Button->font_color = WHITE;
    // Button->icon_align = LEFT_MIDDLE;
    Button->png_buffer = true;
    Button->def_language = language;
}
static void general_language_button_list(void)
{
    resource res = resource_get(ROM_R_2_SET_LANGUAGE_PNG);

    position pos_1 = {{371,424},{190,60}};
    general_language_button_init(&LANGUAGE[LANGUAGE_E_BUTTON],pos_1,res,STR_ENGLISH,language_english);
    pos_1.point.x = 561;
    general_language_button_init(&LANGUAGE[LANGUAGE_K_BUTTON],pos_1,res,STR_KOREAN,language_korean);
    pos_1.point.x = 751;
    general_language_button_init(&LANGUAGE[LANGUAGE_S_BUTTON],pos_1,res,STR_SPAIN,language_spanish);
    
    position pos_2 = {{561,484},{190,60}};
    general_language_button_init(&LANGUAGE[LANGUAGE_V_BUTTON],pos_2,res,STR_VIETNAMESE,language_vietnamese);
     pos_2.point.x = 371;   
    general_language_button_init(&LANGUAGE[LANGUAGE_A_BUTTON],pos_2,res,STR_ARAB,language_arabic);
    pos_2.point.x = 751;  
    general_language_button_init(&LANGUAGE[LANGUAGE_R_BUTTON],pos_2,res,STR_RUSSIAN,language_russian);
}

/* 设置主语言窗口、标题-显示 */
static void general_language_subtitle_display(void)
{
    text Language;
    position pos = {{371,381},{190,46}};
    text_init(&Language,&pos,font_size(SET_OPTION_INFO));
    // Language.font_color = WHITE;
    Language.align = LEFT_MIDDLE;
    Language.erase = true;
    language_text_display(&Language,text_str(STR_LANGUAGE),language_get());
    prev_language->resource.id = ROM_R_2_SET_LANGUAGE_SELECT_PNG;
    prev_language->resource.size = ROM_R_2_SET_LANGUAGE_SELECT_PNG_SIZE;
    button_control_group(LANGUAGE,LANGUAGE_E_BUTTON,LANGUAGE_BUTTON_TOTAL,button_start_using);
}

/* 时间设置-文本-定义
    pos1：输入文本区域
    pos2：标题文本区域
    dt_txt：输入文本内容
    dt_str：标题文本内容 */
static void general_date_time_init(position pos1,position pos2,int dt_txt,char *dt_str)
{
    text DTtxt,DTstr;
    text_init(&DTtxt,&pos1,font_size(SET_OPTION_INFO));
    text_init(&DTstr,&pos2,font_size(SET_OPTION_INFO));
    DTtxt.bg_color = BLACK;
    DTstr.font_color = WHITE;
    char DT_char[5];
    sprintf(DT_char, "%d", dt_txt);  
    text_display(&DTtxt,DT_char);
    text_display(&DTstr,dt_str);
}
static void general_date_display(void)
{
    position pos = {{504,269},{70,60}};
    position str_pos = {{576,269},{28,54}};
    general_date_time_init(pos,str_pos,user_data_get()->user_default_time.day+1,"D");
    pos.point.x = 617;
    str_pos.point.x = 689;
    general_date_time_init(pos,str_pos,user_data_get()->user_default_time.month+1,"M");
    pos.point.x = 733;
    pos.vector.width = 100;
    str_pos.point.x = 835;
    general_date_time_init(pos,str_pos,user_data_get()->user_default_time.year,"Y");
}
static void general_time_display(void)
{
    position pos = {{504,269},{70,60}};
    position str_pos = {{576,269},{28,54}};
    general_date_time_init(pos,str_pos,user_data_get()->user_default_time.hour,"H");
    pos.point.x = 617;
    str_pos.point.x = 689;
    general_date_time_init(pos,str_pos,user_data_get()->user_default_time.minute,"M");
}

static void general_net_time_icon_set(void){
    // if(tuya_get_app_register_status() != E_IPC_ACTIVEATED){
    //     SYSTEM_TIME[NET_SYN_BUTTON].resource.id = ROM_R_2_SET_LANGUAGE_PNG;
    //     SYSTEM_TIME[NET_SYN_BUTTON].resource.size = ROM_R_2_SET_LANGUAGE_PNG_SIZE;
    //     user_data_get()->user_default_mode_conf.user_default_net_time = false;
    // }else{
        SYSTEM_TIME[NET_SYN_BUTTON].font_size = font_size(SET_OPTION_INFO);
        if(user_data_get()->user_default_mode_conf.user_default_net_time == false){
            SYSTEM_TIME[NET_SYN_BUTTON].resource.id = ROM_R_2_SET_LANGUAGE_PNG;
            SYSTEM_TIME[NET_SYN_BUTTON].resource.size = ROM_R_2_SET_LANGUAGE_PNG_SIZE;             
        }else{
            SYSTEM_TIME[NET_SYN_BUTTON].resource.id = ROM_R_2_SET_LANGUAGE_SELECT_PNG;
            SYSTEM_TIME[NET_SYN_BUTTON].resource.size = ROM_R_2_SET_LANGUAGE_SELECT_PNG_SIZE;           
        }
    // }

}

/* 设置网络时间窗口、标题-显示 */
static void general_net_time_subtitle_display(void)
{
    text net_time;
    position pos = {{371,381},{190,46}};
    text_init(&net_time,&pos,font_size(SET_OPTION_INFO));
    net_time.font_color = WHITE;
    net_time.align = LEFT_MIDDLE;
    net_time.erase = true;
    language_text_display(&net_time,text_str(STR_NET_TIME),language_get());
    general_net_time_icon_set();
    button_control_group(LANGUAGE,LANGUAGE_E_BUTTON,LANGUAGE_BUTTON_TOTAL,button_start_disable);
    button_start_using(&SYSTEM_TIME[NET_SYN_BUTTON]);
}

static bool is_open_QR1 = false;
static void QR_code_button_up(const void *arg);
static void time_date_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    // ak_get_localdate(&user_data_get()->user_default_time);


    int time_but = SequenceSearch(SYSTEM_TIME, (Par_Button),SYS_TIME_TOTAL);
    switch (time_but)
    {
    case TIME_BUTTON:
    case DATA_BUTTON:
        if(prev_time_date != (Par_Button))
        {
            button_refresh(prev_time_date,ROM_R_2_SET_TIME_DATE_PNG,ROM_R_2_SET_TIME_DATE_PNG_SIZE,true);
            button_refresh(Par_Button,ROM_R_2_SET_TIME_DATE_SELECT_PNG,ROM_R_2_SET_TIME_DATE_SELECT_PNG_SIZE,true);
            prev_time_date = (Par_Button);
        }

        position pos = {{483,230},{480,128}};
        gui_erase(&pos,0x00);

        if(Par_Button == &SYSTEM_TIME[TIME_BUTTON]){
            general_time_display();
            general_net_time_subtitle_display();
        }
        else
        {
            general_date_display();
            button_start_disable(&SYSTEM_TIME[NET_SYN_BUTTON]);
            general_language_subtitle_display();
        }
        if(is_open_QR1 == true)
            QR_code_button_up(&QR_CODE_BUTTON[OFFICIAL_QR1]);
            
        add_sub_key_button_using();
        break;

    case NET_SYN_BUTTON:
        DEBUG_LOG("--------------->>>NET_TIME \n\r");
        if((user_data_get()->user_default_mode_conf.user_default_net_time = !user_data_get()->user_default_mode_conf.user_default_net_time))
        {
                hint_window_diaplay(STR_TUYA_NET_TIME_TEXT1,STR_TUYA_NET_TIME_TEXT2,true);
                disable_prev_lay_button(backup_button_group,&backup_button_total);
                button_enable(&OPTION_ACK[NET_TIME_ACK_BUTTON]);
        }
        else
        {
            button_refresh(&SYSTEM_TIME[NET_SYN_BUTTON],ROM_R_2_SET_LANGUAGE_PNG,ROM_R_2_SET_LANGUAGE_PNG_SIZE,true);
        }          
        break; 
    default:
        break;
    }
}
/* 时间切换-按键-定义 */
static void general_date_button_init(button *Button,position pos,resource res,enum btn_string_id id)
{
    button_init(Button,&pos,time_date_button_up);
    button_icon_init(Button,&res);
    button_text_init(Button,btn_str(id),font_size(SET_OPTION_INFO));
    Button->font_color = WHITE;
    Button->font_align = CENTER_MIDDLE;
}
static void time_date_button_list(void)
{
    position pos = {{371,253},{110,46}};
    resource res = resource_get(ROM_R_2_SET_TIME_DATE_PNG);
    general_date_button_init(&SYSTEM_TIME[TIME_BUTTON],pos,res,STR_TIME);
    pos.point.y = 299;
    general_date_button_init(&SYSTEM_TIME[DATA_BUTTON],pos,res,STR_DATE);

    position pos_net =  {{371,424},{190,60}};
    resource res_net = resource_get(ROM_R_2_SET_LANGUAGE_PNG);
    general_date_button_init(&SYSTEM_TIME[NET_SYN_BUTTON],pos_net,res_net,STR_AUTO_SET);
    // SYSTEM_TIME[NET_SYN_BUTTON].font_size = 20;
}
static void time_date_button_display(void)
{
    prev_time_date->resource.id = ROM_R_2_SET_TIME_DATE_SELECT_PNG;
    prev_time_date->resource.size = ROM_R_2_SET_TIME_DATE_SELECT_PNG_SIZE;

    if(prev_time_date == &SYSTEM_TIME[TIME_BUTTON]){
        general_time_display();
        general_net_time_subtitle_display();
    }
    else{
        SYSTEM_TIME[TIME_BUTTON].resource.id = ROM_R_2_SET_TIME_DATE_PNG;
        SYSTEM_TIME[TIME_BUTTON].resource.size = ROM_R_2_SET_TIME_DATE_PNG_SIZE;   
        general_date_display();
    }
    button_start_using(&SYSTEM_TIME[TIME_BUTTON]);
    button_start_using(&SYSTEM_TIME[DATA_BUTTON]);
}


static void general_adjust_button_up(const void *arg)
 {
    int Month[MONTH_TOTAL]={31,28,31,30,31,30,31,31,30,31,30,31};
    int adjust_but = SequenceSearch(GENERAL_ADJUST, (Par_Button),GENERAL_ADJUST_TOTAL);
    time_corrected = true;


    if(is_open_QR1 == true && prev_time_date == &SYSTEM_TIME[DATA_BUTTON])
        QR_code_button_up(&QR_CODE_BUTTON[OFFICIAL_QR1]);

    switch (adjust_but)
    {
    case UP_D_BUTTON:
        if(is_leap_feb(user_data_get()->user_default_time))//判断是否是闰年二月
            user_data_get()->user_default_time.day = (user_data_get()->user_default_time.day < 28)?(user_data_get()->user_default_time.day+1):0;
        else
            user_data_get()->user_default_time.day = (user_data_get()->user_default_time.day < (Month[user_data_get()->user_default_time.month]-1))?(user_data_get()->user_default_time.day+1):0;
        general_date_display();
        break;
     case UP_M_BUTTON:
        user_data_get()->user_default_time.month = (user_data_get()->user_default_time.month < 11)?(user_data_get()->user_default_time.month+1):0;
        time_to_correct(&user_data_get()->user_default_time,Month);
        general_date_display();
        break;
    case UP_Y_BUTTON:
        user_data_get()->user_default_time.year = user_data_get()->user_default_time.year < 2037?user_data_get()->user_default_time.year+1:1970;
        time_to_correct(&user_data_get()->user_default_time,Month);
        general_date_display();
        break;
    case UP_H_BUTTON:
        user_data_get()->user_default_time.hour = (user_data_get()->user_default_time.hour < 23)?(user_data_get()->user_default_time.hour+1):0;
        general_time_display();
        break;
    case UP_MI_BUTTON:
        user_data_get()->user_default_time.minute = (user_data_get()->user_default_time.minute < 59)?(user_data_get()->user_default_time.minute+1):0;
        general_time_display();
        break;
    case DOWN_D_BUTTON:
        if(is_leap_feb(user_data_get()->user_default_time))
            user_data_get()->user_default_time.day = (user_data_get()->user_default_time.day > 0)?(user_data_get()->user_default_time.day-1):28;
        else
            user_data_get()->user_default_time.day = (user_data_get()->user_default_time.day > 0)?(user_data_get()->user_default_time.day-1):Month[user_data_get()->user_default_time.month]-1;
        general_date_display();
        break;
    case DOWN_M_BUTTON:
        user_data_get()->user_default_time.month = (user_data_get()->user_default_time.month < 1)?11:(user_data_get()->user_default_time.month-1);
        time_to_correct(&user_data_get()->user_default_time,Month);
        general_date_display();
        break;
    case DOWN_Y_BUTTON:
        user_data_get()->user_default_time.year = user_data_get()->user_default_time.year > 1970?user_data_get()->user_default_time.year-1:2037;
        time_to_correct(&user_data_get()->user_default_time,Month);
        general_date_display();
        break;   
    case DOWN_H_BUTTON:
        user_data_get()->user_default_time.hour = (user_data_get()->user_default_time.hour < 1)?23:(user_data_get()->user_default_time.hour-1);
        general_time_display();
        break;   
    case DOWN_MI_BUTTON:
        user_data_get()->user_default_time.minute = (user_data_get()->user_default_time.minute < 1)?59:(user_data_get()->user_default_time.minute-1);
        general_time_display();
        break;   
    default:
        break;
    }
 }
static void general_adjust_button_leave_down(const void *arg)
 {
    int adjust_but = SequenceSearch(GENERAL_ADJUST, (Par_Button),GENERAL_ADJUST_TOTAL);
    switch (adjust_but)
    {
    case UP_D_BUTTON:
    case UP_M_BUTTON:
    case UP_Y_BUTTON:
    case UP_H_BUTTON:
    case UP_MI_BUTTON:
        button_refresh(Par_Button,ROM_R_2_SET_UP_PNG,ROM_R_2_SET_UP_PNG_SIZE,true);
        break;
    case DOWN_D_BUTTON:
    case DOWN_M_BUTTON:
    case DOWN_Y_BUTTON:
    case DOWN_H_BUTTON:
    case DOWN_MI_BUTTON:
        button_refresh(Par_Button,ROM_R_2_SET_DOWN_PNG,ROM_R_2_SET_DOWN_PNG_SIZE,true);
        break;   
    default:
        break;
    }
 }
 static void general_adjust_button_down(const void *arg)
 {
     
     if(prev_time_date == &SYSTEM_TIME[DATA_BUTTON])
     {
         if(SequenceSearch(GENERAL_ADJUST, (Par_Button),UP_H_BUTTON) < DOWN_D_BUTTON)
            button_refresh(Par_Button,ROM_R_2_SET_UP_SELECT_PNG,ROM_R_2_SET_UP_SELECT_PNG_SIZE,true);
        else
            button_refresh(Par_Button,ROM_R_2_SET_DOWN_SELECT_PNG,ROM_R_2_SET_DOWN_SELECT_PNG_SIZE,true);
     }
     else if(prev_time_date == &SYSTEM_TIME[TIME_BUTTON])
     {
         if(SequenceSearch(GENERAL_ADJUST, (Par_Button),GENERAL_ADJUST_TOTAL) < DOWN_H_BUTTON)
            button_refresh(Par_Button,ROM_R_2_SET_UP_SELECT_PNG,ROM_R_2_SET_UP_SELECT_PNG_SIZE,true);
        else
            button_refresh(Par_Button,ROM_R_2_SET_DOWN_SELECT_PNG,ROM_R_2_SET_DOWN_SELECT_PNG_SIZE,true);
     }
 }
 /* 常规设置-加减类型按键-定义 */
static void general_adjust_button_init(button *Button,position pos,resource res,align but_align)
 {
    button_init(Button,&pos,general_adjust_button_up);
    button_icon_init(Button,&res);
    // Button->bg_color = BLUE;
    Button->down = general_adjust_button_down;
    Button->long_down = general_adjust_button_up;
    Button->leave_down = general_adjust_button_leave_down;
    Button->icon_align = but_align;
 }

 static void general_adjust_button_list(void)
 {
    position pos_a = {{489,214},{100,54}};
    position pos_s = {{489,329},{100,60}};
    resource res_a = resource_get(ROM_R_2_SET_UP_PNG);
    resource res_s = resource_get(ROM_R_2_SET_DOWN_PNG);
    general_adjust_button_init(&GENERAL_ADJUST[UP_D_BUTTON],pos_a,res_a,CENTER_BOTTOM);
    pos_a.point.x = 603;
    general_adjust_button_init(&GENERAL_ADJUST[UP_M_BUTTON],pos_a,res_a,CENTER_BOTTOM);   
    pos_a.point.x = 733;
    general_adjust_button_init(&GENERAL_ADJUST[UP_Y_BUTTON],pos_a,res_a,CENTER_BOTTOM);
    pos_a.point.x = 489;
    general_adjust_button_init(&GENERAL_ADJUST[UP_H_BUTTON],pos_a,res_a,CENTER_BOTTOM);
    pos_a.point.x = 603;   
    general_adjust_button_init(&GENERAL_ADJUST[UP_MI_BUTTON],pos_a,res_a,CENTER_BOTTOM);

    general_adjust_button_init(&GENERAL_ADJUST[DOWN_D_BUTTON],pos_s,res_s,CENTER_TOP);
    pos_s.point.x = 603;
    general_adjust_button_init(&GENERAL_ADJUST[DOWN_M_BUTTON],pos_s,res_s,CENTER_TOP);
    pos_s.point.x = 733;
    general_adjust_button_init(&GENERAL_ADJUST[DOWN_Y_BUTTON],pos_s,res_s,CENTER_TOP);
    pos_s.point.x = 489;
    general_adjust_button_init(&GENERAL_ADJUST[DOWN_H_BUTTON],pos_s,res_s,CENTER_TOP);
    pos_s.point.x = 603;   
    general_adjust_button_init(&GENERAL_ADJUST[DOWN_MI_BUTTON],pos_s,res_s,CENTER_TOP); 
 }

static void general_interface_list(void)
{
    general_tuya_button_init();
    general_device_id_button_list();
    general_language_button_list();
    time_date_button_list();
    general_adjust_button_list();
}
static void general_interface_display(void)
{
    
    is_open_QR1 = false;
    button_start_using(&QR_CODE_BUTTON[OFFICIAL_QR2]);
    
    general_tuya_resource_display();//4ms
    
    button_enable(&tuya_app_button);
    general_device_id_subtitle_display();//15ms
    
    general_language_subtitle_display();//93ms
    
    time_date_button_display();//17ms
    
    add_sub_key_button_using();//17ms
    
}
#endif
#ifdef Set_Sound
static void sound_porperty_display(enum sound_source_dev id)
{
    text call_vol,call_mel,talk_vol,speak_sen,mic_sen;
    char  val[4];
    position pos = {{728,176},{145,60}};

    text_init(&call_vol,&pos,font_size(SET_OPTION_INFO));
    pos.point.y = 254;
    text_init(&call_mel,&pos,font_size(SET_OPTION_INFO));
    pos.point.y = 332;
    text_init(&talk_vol,&pos,font_size(SET_OPTION_INFO));
    pos.point.y = 410;
    text_init(&speak_sen,&pos,font_size(SET_OPTION_INFO));
    pos.point.y = 488;
    text_init(&mic_sen,&pos,font_size(SET_OPTION_INFO));

    position era = {{755,182},{70,380}};
    gui_erase(&era,0x00);

    sprintf(val, "%2d", user_data_get()->SOUR_PRO[id].sound.call_vol);   
    text_display(&call_vol,val);

    sprintf(val, "%2d", user_data_get()->SOUR_PRO[id].sound.call_mel);   
    text_display(&call_mel,val);

    sprintf(val, "%2d", user_data_get()->SOUR_PRO[id].sound.talk_vol);   
    text_display(&talk_vol,val);

    sprintf(val, "%2d", user_data_get()->SOUR_PRO[id].sound.speak_sen);   
    text_display(&speak_sen,val);

    sprintf(val, "%2d", user_data_get()->SOUR_PRO[id].sound.mic_sen);   
    text_display(&mic_sen,val);
}
/* 切换至要设置的声音源 */
static void sound_source_display(button *Button)
{
    static int source_id = 0;//记录当前输入源id
    text  sound_source;
    int str_id = 0;
    position str_pos = {{728,102},{145,60}};
    text_init(&sound_source,&str_pos,font_size(SET_OPTION_INFO));
    gui_erase(&str_pos,0x00);
    sound_source.align = CENTER_MIDDLE;
    // sound_source.bg_color = BLACK;
    if(Button == &SOUND_SOURCE[LEFT_SWITCH_BUTTON])
    {
        source_id = ((source_id == SOURCE_DEVICE)?(SOURCE_CAM2):(source_id-1));
    }
    else if(Button == &SOUND_SOURCE[RIGHT_SWITCH_BUTTON])
    {
        source_id = ((source_id == SOURCE_CAM2)?(SOURCE_DEVICE):(source_id+1));
    }
    DEBUG_LOG("source_id------------->>>%d\n\r",source_id);
    cur_sound_sour = source_id;
    switch (source_id)
    {
    case SOURCE_DEVICE:
        str_id = STR_INTERCOM;
        sound_source.font_size = font_size(SET_OPTION_INFO) - (language_get() == language_spanish ?  4 : 0);
        if(language_get() == language_english)
        {
            text_display(&sound_source,"Intercom");
            sound_porperty_display(source_id);
            return;
        }
        break;
    case SOURCE_CAM1:
        str_id = STR_CAM1;
        break;
     case SOURCE_CAM2:
        str_id = STR_CAM2;
        break;   

    default:
        break;
    }
    language_text_display(&sound_source,text_str(str_id),language_get());
    sound_porperty_display(source_id);
}
/* 声音源 相关设置-文本标题 */
static void sound_text_display(void)
{
    text call_vol,call_mel,talk_vol,speak_sen,mic_sen;
    position str_pos = {{358,192},{256,42}};

    text_init(&call_vol,&str_pos,font_size(SET_OPTION_INFO));
    call_vol.align = language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;
    // call_vol.bg_color = BLACK;
    str_pos.point.y = 265;
    text_init(&call_mel,&str_pos,font_size(SET_OPTION_INFO));
    call_mel.align = language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;

    str_pos.point.y = 344;
    text_init(&talk_vol,&str_pos,font_size(SET_OPTION_INFO));
    talk_vol.align = language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;

    str_pos.point.y = 423;
    text_init(&speak_sen,&str_pos,font_size(SET_OPTION_INFO));
    speak_sen.align = language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;

    str_pos.point.y = 502;
    text_init(&mic_sen,&str_pos,font_size(SET_OPTION_INFO));
    mic_sen.align = language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;

    language_text_display(&call_vol,text_str(STR_CALL_VOLUME),language_get());
    language_text_display(&call_mel,text_str(STR_CALL_MELODY),language_get());
    language_text_display(&talk_vol, text_str(STR_TALK_VOLUME),language_get());
    language_text_display(&speak_sen, text_str(STR_SPEAL_SEN),language_get());
    language_text_display(&mic_sen,text_str (STR_MIC_SEN),language_get());
}
static void sound_switch_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    if(Par_Button == &SOUND_SOURCE[LEFT_SWITCH_BUTTON])
        button_refresh(Par_Button,ROM_R_2_SET_LEFT_SWITCH_PNG,ROM_R_2_SET_LEFT_SWITCH_PNG_SIZE,true);        
    else 
        button_refresh(Par_Button,ROM_R_2_SET_RIGHT_SWITCH_PNG,ROM_R_2_SET_RIGHT_SWITCH_PNG_SIZE,true);
}
static void sound_switch_button_down(const void *arg)
{
    if(Par_Button == &SOUND_SOURCE[LEFT_SWITCH_BUTTON])
        button_refresh(Par_Button,ROM_R_2_SET_LEFT_SWITCH_SELECT_PNG,ROM_R_2_SET_LEFT_SWITCH_SELECT_PNG_SIZE,true);
    else 
        button_refresh(Par_Button,ROM_R_2_SET_RIGHT_SWITCH_SELECT_PNG,ROM_R_2_SET_RIGHT_SWITCH_SELECT_PNG_SIZE,true);
        sound_source_display(Par_Button);
}
/* 声音源切换按键-初始化 */
static void sound_switch_button_init(button *Button,position pos,resource res)
{
    button_init(Button,&pos,sound_switch_button_up);
    button_icon_init(Button,&res);
    Button->down = sound_switch_button_down;
    Button->leave_down = sound_switch_button_up;
}
static void sound_switch_button_list(void)
{
    position pos = {{656,102},{70,60}};
    resource res_l = resource_get(ROM_R_2_SET_LEFT_SWITCH_PNG);
    resource res_r = resource_get(ROM_R_2_SET_RIGHT_SWITCH_PNG);
    sound_switch_button_init(&SOUND_SOURCE[LEFT_SWITCH_BUTTON],pos,res_l);
    pos.point.x = 876;
    sound_switch_button_init(&SOUND_SOURCE[RIGHT_SWITCH_BUTTON],pos,res_r);
}
static void switch_button_using(void)
{
    button_start_using(&SOUND_SOURCE[LEFT_SWITCH_BUTTON]);
    button_start_using(&SOUND_SOURCE[RIGHT_SWITCH_BUTTON]);
}
static bool play_melody = false;
static void sound_adjust_button_up(const void *arg)
 {
    but_press_time = 10;
    int adjust_but = SequenceSearch(SOUND_ADJUST, (Par_Button),SOUND_ADJUST_TOTAL);
    switch (adjust_but)
    {
        case CALL_VOL_SUB:
        DEBUG_LOG("--------------->>>CALL_VOL_SUB \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_vol > 1)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_vol -= 1;
        play_melody = true;
            break;
        case CALL_MEL_SUB:
        DEBUG_LOG("--------------->>>CALL_MEL_SUB \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_mel > 1)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_mel -= 1;
        play_melody = true;
            break;
        case TALK_VOL_SUB:
        DEBUG_LOG("--------------->>>TALK_VOL_SUB \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.talk_vol > 1)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.talk_vol -= 1;
            break;
        case SPEAK_SEN_SUB:
        DEBUG_LOG("--------------->>>SPEAK_SEN_SUB \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.speak_sen > 1)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.speak_sen -= 1;
            break;
        case MIC_SEN_SUB:
        DEBUG_LOG("--------------->>>MIC_SEN_SUB \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.mic_sen > 1)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.mic_sen -= 1;
            break;
        case CALL_VOL_ADD:
        DEBUG_LOG("--------------->>>CALL_VOL_ADD \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_vol < 6)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_vol +=  1;
        play_melody = true;
            break;
        case CALL_MEL_ADD:
        DEBUG_LOG("--------------->>>CALL_MEL_ADD \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_mel < 6)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_mel += 1;
        play_melody = true;
            break;
        case TALK_VOL_ADD:
        DEBUG_LOG("--------------->>>TALK_VOL_ADD \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.talk_vol < 6)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.talk_vol += 1;
            break;   
        case SPEAK_SEN_ADD:
        DEBUG_LOG("--------------->>>SPEAK_SEN_ADD \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.speak_sen < 2)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.speak_sen += 1;
            break;   
        case MIC_SEN_ADD:
            DEBUG_LOG("--------------->>>MIC_SEN_ADD \n\r");
        if(user_data_get()->SOUR_PRO[cur_sound_sour].sound.mic_sen < 2)
        user_data_get()->SOUR_PRO[cur_sound_sour].sound.mic_sen += 1;
            break;   
        default:
            break;
    }
    sound_porperty_display(cur_sound_sour);    
 }
static void sound_adjust_button_leave_down(const void *arg)
 {
    int adjust_but = SequenceSearch(SOUND_ADJUST, (Par_Button),SOUND_ADJUST_TOTAL);
    switch (adjust_but)
    {
        case CALL_VOL_SUB:
        case TALK_VOL_SUB:
        case SPEAK_SEN_SUB:
        case MIC_SEN_SUB:
            button_refresh(Par_Button,ROM_R_2_SET_ADJUST_SUB_PNG,ROM_R_2_SET_ADJUST_SUB_PNG_SIZE,true);
            break;
        case CALL_MEL_SUB:
            button_refresh(Par_Button,ROM_R_2_SET_LEFT_ADJUST_PNG,ROM_R_2_SET_LEFT_ADJUST_PNG_SIZE,true);
            break;
        case CALL_MEL_ADD:
            button_refresh(Par_Button,ROM_R_2_SET_RIGHT_ADJUST_PNG,ROM_R_2_SET_RIGHT_ADJUST_PNG_SIZE,true);
            break;
        case CALL_VOL_ADD:
        case TALK_VOL_ADD:
        case SPEAK_SEN_ADD:
        case MIC_SEN_ADD:
            button_refresh(Par_Button,ROM_R_2_SET_ADJUST_ADD_PNG,ROM_R_2_SET_ADJUST_ADD_PNG_SIZE,true);
            break;   
        default:
            break;
    }
 }
static void sound_adjust_button_down(const void *arg)
 {
     if(Par_Button == &SOUND_ADJUST[CALL_MEL_SUB] || Par_Button == &SOUND_ADJUST[CALL_MEL_ADD])
     {
                  DEBUG_LOG("--------------->>>%s \n\r",__func__);
         if(Par_Button == &SOUND_ADJUST[CALL_MEL_SUB])
            button_refresh(Par_Button,ROM_R_2_SET_LEFT_SWITCH_SELECT_PNG,ROM_R_2_SET_LEFT_SWITCH_SELECT_PNG_SIZE,true);
        else
            button_refresh(Par_Button,ROM_R_2_SET_RIGHT_SWITCH_SELECT_PNG,ROM_R_2_SET_RIGHT_SWITCH_SELECT_PNG_SIZE,true);
     }
     else
     {
         if(SequenceSearch(SOUND_ADJUST, (Par_Button),SOUND_ADJUST_TOTAL) < CALL_VOL_ADD)
            button_refresh(Par_Button,ROM_R_2_SET_ADJUST_SUB_SELECT_PNG,ROM_R_2_SET_ADJUST_SUB_SELECT_PNG_SIZE,true);
        else
            button_refresh(Par_Button,ROM_R_2_SET_ADJUST_ADD_SELECT_PNG,ROM_R_2_SET_ADJUST_ADD_SELECT_PNG_SIZE,true);
     }
 }
 /* 声音属性-设置按键-初始化 */
static void sound_adjust_button_init(button *Button,position pos,resource res)
 {
    button_init(Button,&pos,sound_adjust_button_up);
    button_icon_init(Button,&res);
    Button->down = sound_adjust_button_down;
    Button->leave_down = sound_adjust_button_leave_down;
 }
static void sound_adjust_button_list(void)
 {
        //  resource res_s = resource_get(ROM_R_2_SET_LEFT_ADJUST_PNG);
    // resource res_a = resource_get(ROM_R_2_SET_RIGHT_ADJUST_PNG);
    position pos_s = {{656,176},{70,60}};
    position pos_a = {{876,176},{70,60}};
    resource res_s = resource_get(ROM_R_2_SET_ADJUST_SUB_PNG);
    resource res_a = resource_get(ROM_R_2_SET_ADJUST_ADD_PNG);
    sound_adjust_button_init(&SOUND_ADJUST[CALL_VOL_SUB],pos_s,res_s);
    SOUND_ADJUST[CALL_VOL_SUB].button_tone = -1;
    pos_s.point.y = 332;
    sound_adjust_button_init(&SOUND_ADJUST[TALK_VOL_SUB],pos_s,res_s);
    pos_s.point.y = 410;
    sound_adjust_button_init(&SOUND_ADJUST[SPEAK_SEN_SUB],pos_s,res_s);
    pos_s.point.y = 488;   
    sound_adjust_button_init(&SOUND_ADJUST[MIC_SEN_SUB],pos_s,res_s);
    pos_s.point.y = 254;
    res_s.id = ROM_R_2_SET_LEFT_ADJUST_PNG;
    res_s.size = ROM_R_2_SET_LEFT_ADJUST_PNG_SIZE;   
    sound_adjust_button_init(&SOUND_ADJUST[CALL_MEL_SUB],pos_s,res_s);  
    SOUND_ADJUST[CALL_MEL_SUB].button_tone = -1; 

    sound_adjust_button_init(&SOUND_ADJUST[CALL_VOL_ADD],pos_a,res_a);
    SOUND_ADJUST[CALL_VOL_ADD].button_tone = -1;
    pos_a.point.y = 332;
    sound_adjust_button_init(&SOUND_ADJUST[TALK_VOL_ADD],pos_a,res_a);
    pos_a.point.y = 410;
    sound_adjust_button_init(&SOUND_ADJUST[SPEAK_SEN_ADD],pos_a,res_a);
    pos_a.point.y = 488;   
    sound_adjust_button_init(&SOUND_ADJUST[MIC_SEN_ADD],pos_a,res_a); 
    pos_a.point.y = 254;
    res_a.id = ROM_R_2_SET_RIGHT_ADJUST_PNG;
    res_a.size = ROM_R_2_SET_RIGHT_ADJUST_PNG_SIZE;   
    sound_adjust_button_init(&SOUND_ADJUST[CALL_MEL_ADD],pos_a,res_a);
    SOUND_ADJUST[CALL_MEL_ADD].button_tone = -1;
 }
static void sound_interface_list(void)
{
    sound_adjust_button_list();
    sound_switch_button_list();
}
static void sound_interface_display(void)
{
        switch_button_using();
        sound_text_display();
        sound_source_display(NULL);
        add_sub_key_button_using();   
}
#endif

#ifdef Set_Connect
static void wifi_head_display(void)
{
    text wifi;
    position pos = {{357,146},{165,34}};
    text_init(&wifi,&pos,font_size(SET_OPTION_INFO));
    wifi.align = LEFT_MIDDLE;
    char wifi_srt[6];
    sprintf(wifi_srt,"%s","WIFI");
    text_display(&wifi,wifi_srt);

    text mac;
    position mac_pos = {{357,178},{423,28}};
    text_init(&mac,&mac_pos,font_size(HOME_DATE));
    mac.align = LEFT_MIDDLE;
    mac.font_color = WHITE;
    char mac_srt[30];
    sprintf(mac_srt,"%s","WI-FI MAC: AA:00:00:BB:F#:86");
    text_display(&mac,mac_srt);

    position line_pos = {{352,207},{612,1}};
    draw_rect(&line_pos,WHITE);
}
static void add_manually_button_up(const void* arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    erase_bg();
    window_headline_diaplay(STR_MANUALLY);
    button_control_group(SWITCH_BUT,WIFI_POWER,SWITCH_TOTAL,button_disable);
    button_disable(&manually_button);
    button_start_using(&RETURN_BUTTON[CONNECT_RETURN]);
    button_control_group(OPTION_ACK,CONNECT_YES_BUTTON,NET_TIME_ACK_BUTTON,button_start_using);
    input_subtitle_diaplay(STR_WIFI_NAME,STR_PASSWORD);
    button_control_group(INPUT_BUT,CONNECT_WIFI_NAME,INPUT_TYPE_TOTAL,button_start_using);
    button_start_using(&INPUT_SHOW_BUT[CONNECT_WIFI_PASSW]);
    password_display(&input_buffer[CONNECT_WIFI_NAME_BOX]);
    password_display(&input_buffer[CONNECT_WIFI_PASSW_BOX]);
    position pos = {{767,212},{48,48}};
    draw_rect(&pos,BLACK);
}
static void add_manually_button_down(const void* arg)
{
    button_refresh(&manually_button,ROM_R_2_SET_ADD_WIFI_ON_PNG,ROM_R_2_SET_ADD_WIFI_ON_PNG_SIZE,true);
}
static void add_manually_button_leave_down(const void* arg)
{
    button_refresh(&manually_button,ROM_R_2_SET_ADD_WIFI_PNG,ROM_R_2_SET_ADD_WIFI_PNG_SIZE,true);
}

static void add_manually_button_init(void)
{
    position pos = {{847,219},{60,30}};
    resource res = resource_get(ROM_R_2_SET_ADD_WIFI_PNG);
    button_init(&manually_button,&pos,add_manually_button_up);
    button_icon_init(&manually_button,&res);
    manually_button.leave_down = add_manually_button_leave_down;
    manually_button.down =add_manually_button_down;
}
static void connect_switch_list(void)
{
    position pos = {{834,146},{86,45}};
    resource res = resource_get(ROM_R_2_SET_SWITCH_OFF_PNG);
    switch_button_init(&SWITCH_BUT[WIFI_POWER],pos,res);    
}

static void connect_button_list(void)
{
    connect_switch_list();
    add_manually_button_init();
}
#endif


#ifdef Set_Mode
/* 离家模式延时、监控时间、开门时间-显示效果 */
static void mode_general_time_display(void)
{
    text delay_time_str;
    text m_time_str;
    text d_time_str;
    position str_pos = {{736,310},{100,60}};
    // position general_time_display = {{736,311},{100,200}};
    char delay_str[5];
    char mon_str[4];
    char door_str[4];

    text_init(&delay_time_str,&str_pos,font_size(SET_OPTION_INFO));   
    delay_time_str.erase = true;
    delay_time_str.align = CENTER_MIDDLE;
    str_pos.point.y = 380;
    text_init(&m_time_str,&str_pos,font_size(SET_OPTION_INFO));   
    m_time_str.erase = true;
    m_time_str.align = CENTER_MIDDLE;
    str_pos.point.y = 450;
    text_init(&d_time_str,&str_pos,font_size(SET_OPTION_INFO));      
    d_time_str.erase = true; 
    d_time_str.align = CENTER_MIDDLE;
    // gui_erase(&general_time_display,0x00);
    sprintf(delay_str, "%d", user_data_get()->user_default_mode_conf.user_default_absent_delay);   
    sprintf(mon_str, "%2d", user_data_get()->user_default_mode_conf.time_conf.mon_time);   
    sprintf(door_str, "%g", user_data_get()->user_default_mode_conf.time_conf.door_time == 0 ? 0.2 : user_data_get()->user_default_mode_conf.time_conf.door_time);   
    // delay_time_str.bg_color = BLUE;
    text_display(&delay_time_str, delay_str);
    text_display(&m_time_str, mon_str);
    text_display(&d_time_str, door_str);
}
/* mode主设置-显示标题 */
static void mode_off_no_text_init(void)
{
    text off_up;
    text off_down;
    text on_up;
    text on_down;

    position pos_off = {{626,160},{112,50}};
    position pos_on = {{832,160},{110,50}};

    char *off_str[LANGUAGE_TOTAL] = {"OFF","OFF","APAGADO","عدم تشغيل ","Tắt","Отключено"};
    char *no_str[LANGUAGE_TOTAL] = {"ON","ON","ENCENDIDO","تشعيل ","Mở","Включено"};

    int str_size = language_get() == language_russian || language_get() == language_spanish ? font_size(SET_OPTION_INFO) - 2 : font_size(SET_OPTION_INFO);
    text_init(&off_up,&pos_off,str_size);
    off_up.font_color = 0xFF828582;
    // off_up.bg_color = BLUE;
    pos_off.point.y = 237;
    text_init(&off_down,&pos_off,str_size);
    off_down.font_color = 0xFF828582;

    text_init(&on_up,&pos_on,str_size);
    on_up.font_color = 0xFF828582;
    pos_on.point.y = 237;
    text_init(&on_down,&pos_on,str_size); 
    on_down.font_color = 0xFF828582;


    off_up.def_language = off_down.def_language = on_up.def_language = on_down.def_language = language_get();

    text_display(&off_up,off_str[language_get()]);
    text_display(&off_down,off_str[language_get()]);
    text_display(&on_up,no_str[language_get()]);
    text_display(&on_down,no_str[language_get()]);
}
static void mode_text_display(void)
{
    text always_on;

    text door_time;

    position str_pos = {{358,240},{280,42}};
    str_pos.vector.width = language_get() ==  language_arabic ?  220 : 280;
    mode_off_no_text_init();


    text_init(&always_on,&str_pos,font_size(SET_OPTION_INFO));
    always_on.align = language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;   
    // always_on.bg_color = BLACK;



#if SD_CARD
    text auto_recode;
    str_pos.point.y = 334;
    text_init(&auto_recode,&str_pos,font_size(SET_OPTION_INFO));
    auto_recode.align = LEFT_BOTTOM;    
    endif(&auto_recode, (STR_AUTO_RECODE));
#else
    text absent_delay;
    str_pos.point.y = language_get() == language_russian ? 307 : 322;
    text_init(&absent_delay,&str_pos,language_get() == language_russian ? font_size(SET_OPTION_INFO) : font_size(SET_OPTION_INFO));
    absent_delay.align =  language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;   
    language_text_display(&absent_delay,text_str(STR_ABSENT_DELAY),language_get());

    if(language_get() == language_russian)
    {
        str_pos.point.y = 337;
        text_init(&absent_delay, &str_pos, font_size(SET_OPTION_INFO));
        absent_delay.align = LEFT_BOTTOM;
        text_display(&absent_delay, "отсутствия");

    }
#endif

    str_pos.point.y = 462;
    text_init(&door_time,&str_pos,font_size(SET_OPTION_INFO));
    door_time.align = language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;   



    language_text_display(&always_on,text_str(STR_ALWAYS_ON),language_get());
    language_text_display(&door_time,text_str(STR_DOOR_TIME),language_get());

    mode_general_time_display();


    if(language_get() != language_russian){
        if(language_get() ==  language_spanish)
        {
            text auto_image1;
            text auto_image2;

            str_pos.point.y = 144;
            text_init(&auto_image1,&str_pos,font_size(SET_OPTION_INFO));
            auto_image1.align = LEFT_BOTTOM;   

            str_pos.point.y = 174;
            text_init(&auto_image2,&str_pos,font_size(SET_OPTION_INFO));
            auto_image2.align = LEFT_BOTTOM;   

            text_display(&auto_image1,"Captura automática");   
            text_display(&auto_image2,"de imágenes");                 
        }
        else
        {
            text auto_image;
            str_pos.point.y = 168;
            text_init(&auto_image,&str_pos,font_size(SET_OPTION_INFO));
            auto_image.align = language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;   
            language_text_display(&auto_image,text_str(STR_AUTO_IMAGE),language_get());               
        }


        text monitoring;
        str_pos.point.y = 392;
        text_init(&monitoring,&str_pos,font_size(SET_OPTION_INFO));
        monitoring.align = language_get() ==  language_arabic ? RIGHT_BOTTOM : LEFT_BOTTOM;   
        language_text_display(&monitoring,text_str(STR_MONITORING),language_get());
    }
    else
    {
        text auto_image1;
        text auto_image2;

        str_pos.point.y = 144;
        text_init(&auto_image1,&str_pos,font_size(SET_OPTION_INFO));
        auto_image1.align = LEFT_BOTTOM;   

        str_pos.point.y = 174;
        text_init(&auto_image2,&str_pos,font_size(SET_OPTION_INFO));
        auto_image2.align = LEFT_BOTTOM;   

        text_display(&auto_image1,"Автоматическая запись");   
        text_display(&auto_image2,"изображения");   

        text monitoring1;
        text monitoring2;
        str_pos.point.y = 378;
        text_init(&monitoring1,&str_pos,font_size(SET_OPTION_INFO));
        monitoring1.align = LEFT_BOTTOM;    

        str_pos.point.y = 410;
        text_init(&monitoring2,&str_pos,font_size(SET_OPTION_INFO));
        monitoring2.align = LEFT_BOTTOM;    

        text_display(&monitoring1,"Время просмотра");
        text_display(&monitoring2,"без активации блока вызова");

    }
}

static bool unlock_time_modify = false;
static void mode_adjust_button_up(const void *arg)
 {
    int adjust_but = SequenceSearch(MODE_ADJUST, (Par_Button),MODE_ADJUST_TOTAL);
    switch (adjust_but)
    {
        case ABSENT_DELAY_SUB:
            DEBUG_LOG("--------------->>>ABSENT_DELAY_SUB \n\r");
            if(user_data_get()->user_default_mode_conf.user_default_absent_delay > 15)
                user_data_get()->user_default_mode_conf.user_default_absent_delay -= 15;
            user_data_get()->security.absent_start_time = os_get_ms();
            break;
        case MONITOR_SUB:
            DEBUG_LOG("--------------->>>MONITOR_SUB \n\r");
            if(user_data_get()->user_default_mode_conf.time_conf.mon_time > 120)
                user_data_get()->user_default_mode_conf.time_conf.mon_time = 120;
            else if(user_data_get()->user_default_mode_conf.time_conf.mon_time > 30)
                user_data_get()->user_default_mode_conf.time_conf.mon_time /= 2; 
            break;
        case DOOR_TIME_SUB:
            unlock_time_modify = true;
            if(user_data_get()->user_default_mode_conf.time_conf.door_time > 0)
                user_data_get()->user_default_mode_conf.time_conf.door_time --;

            DEBUG_LOG("--------------->>>DOOR_TIME_SUB \n\r");
            break;
        case ABSENT_DELAY_ADD:
            if(user_data_get()->user_default_mode_conf.user_default_absent_delay < 60)
                user_data_get()->user_default_mode_conf.user_default_absent_delay += 15;
            user_data_get()->security.absent_start_time = os_get_ms();
            DEBUG_LOG("--------------->>>ABSENT_DELAY_ADD \n\r");
            break;
        case MONITOR_ADD:
            if(user_data_get()->user_default_mode_conf.time_conf.mon_time < 120)
                user_data_get()->user_default_mode_conf.time_conf.mon_time *= 2;
            else if(user_data_get()->user_default_mode_conf.time_conf.mon_time < 180)
                user_data_get()->user_default_mode_conf.time_conf.mon_time = 180;
            DEBUG_LOG("--------------->>>MONITOR_ADD \n\r");
            break;
        case DOOR_TIME_ADD:
            unlock_time_modify = true;
            if(user_data_get()->user_default_mode_conf.time_conf.door_time < 9)
                user_data_get()->user_default_mode_conf.time_conf.door_time ++;
            
            DEBUG_LOG("--------------->>>DOOR_TIME_ADD \n\r");
            break;
        default:
            break;
    }
    mode_general_time_display();
 }
static void mode_adjust_button_leave_down(const void *arg)
 {
    int adjust_but = SequenceSearch(MODE_ADJUST, (Par_Button),MODE_ADJUST_TOTAL);
    switch (adjust_but)
    {
        case ABSENT_DELAY_SUB:
        case MONITOR_SUB:
        case DOOR_TIME_SUB:
            button_refresh(Par_Button,ROM_R_2_SET_ADJUST_SUB_PNG,ROM_R_2_SET_ADJUST_SUB_PNG_SIZE,true);
            break;
        case ABSENT_DELAY_ADD:
        case MONITOR_ADD:
        case DOOR_TIME_ADD:
            button_refresh(Par_Button,ROM_R_2_SET_ADJUST_ADD_PNG,ROM_R_2_SET_ADJUST_ADD_PNG_SIZE,true);
            break;
        default:
            break;
    }
 }
static void mode_adjust_button_down(const void *arg)
{
     if(SequenceSearch(MODE_ADJUST, (Par_Button),MODE_ADJUST_TOTAL) < MONITOR_ADD)
            button_refresh(Par_Button,ROM_R_2_SET_ADJUST_SUB_SELECT_PNG,ROM_R_2_SET_ADJUST_SUB_SELECT_PNG_SIZE,true);
     else
            button_refresh(Par_Button,ROM_R_2_SET_ADJUST_ADD_SELECT_PNG,ROM_R_2_SET_ADJUST_ADD_SELECT_PNG_SIZE,true);
}
/* mode主设置-加减键-初始化 */
static void mode_adjust_button_init(button *Button,position pos,resource res)
 {
    button_init(Button,&pos, mode_adjust_button_up);
    button_icon_init(Button,&res);
    Button->down = mode_adjust_button_down;
    Button->leave_down = mode_adjust_button_leave_down;
 }
static void mode_adjust_button_list(void)
 {
    position pos_s = {{666,313},{70,60}};
    position pos_a = {{836,313},{70,60}};
    resource res_s = resource_get(ROM_R_2_SET_ADJUST_SUB_PNG);
    resource res_a = resource_get(ROM_R_2_SET_ADJUST_ADD_PNG);

    mode_adjust_button_init(&MODE_ADJUST[ABSENT_DELAY_SUB],pos_s,res_s);
    pos_s.point.y = 383;
    mode_adjust_button_init(&MODE_ADJUST[MONITOR_SUB],pos_s,res_s);
    pos_s.point.y = 453;
    mode_adjust_button_init(&MODE_ADJUST[DOOR_TIME_SUB],pos_s,res_s);

    mode_adjust_button_init(&MODE_ADJUST[ABSENT_DELAY_ADD],pos_a,res_a);
    pos_a.point.y = 383;   
    mode_adjust_button_init(&MODE_ADJUST[MONITOR_ADD],pos_a,res_a);
    pos_a.point.y = 453;   
    mode_adjust_button_init(&MODE_ADJUST[DOOR_TIME_ADD],pos_a,res_a);
 }

static void mode_switch_button_list(void)
 {
    position pos = {{744,237},{86,45}};
    resource res[2]= {resource_get(ROM_R_2_SET_SWITCH_OFF_PNG),resource_get(ROM_R_2_SET_SWITCH_ON_PNG)};
    switch_button_init(&SWITCH_BUT[ALWAYS_NO],pos,res[user_data_get()->user_default_mode_conf.user_default_always]);
    pos.point.y = 160;
    switch_button_init(&SWITCH_BUT[AUTO_IMAGE],pos,res[user_data_get()->user_default_mode_conf.user_default_snap]);
    
#if SD_CARD
    pos.point.y = 323;
    switch_button_init(&SWITCH_BUT[AUTO_RECODE],pos,res[user_data_get()->user_default_mode_conf.user_default_record]);
#endif
 }
static void mode_interface_list(void)
{
    mode_adjust_button_list();
    mode_switch_button_list();
}
static void mode_interface_display(void)
 {

    mode_text_display();
    add_sub_key_button_using();   

 }
#endif

#ifdef Set_Other
static void password_display(char **p)
{
    if(*p != NULL)
    {        
        text pass;
        position pos = {{546,235},{282,48}};
        char *input_hide_str = NULL;
        char string[64] = {0};  

        bool pass_is =  (*p == input_buffer[ALARM_OLD_PASS_BOX])?is_old_hide:is_new_hide;           
        if(*p == input_buffer[ALARM_NEW_PASS_BOX])     pos.point.y = 318;
        else if(*p == input_buffer[CONNECT_WIFI_NAME_BOX])    pass_is = false;          
        else if(*p == input_buffer[CONNECT_WIFI_PASSW_BOX]){
            pos.point.y = 318;
            pass_is = is_wifi_pass_hide;            
        }        
        text_init(&pass,&pos,24);
        pass.font_color = WHITE;
        pass.bg_color = BLACK;
        pass.align = LEFT_BOTTOM;
        pass.offset.x = 10;        
        gui_erase(&pos,0x00);      
        if(pass_is)
        {
            for(int i = 0;i<strlen(*p);i++)
            {
                string[i] = '*';
            }            
            input_hide_str = string;
            text_display(&pass,input_hide_str); 
        }
        else
        {
            text_display(&pass,*p);  
        }
    }
}
static void other_dajust_text_display(void){
    text brightness;
    text brightness_vol;
    position str_pos = {{382,138},{276,46}};
    position vol_pos = {{770,123},{100,60}};
    char bri_str[5];
    text_init(&brightness_vol,&vol_pos,font_size(SET_OPTION_INFO));   
    brightness_vol.erase = true;
    // brightness_vol.bg_color = BLUE;
    brightness_vol.align = CENTER_MIDDLE;
    sprintf(bri_str, "%d %%", user_data_get()->user_default_mode_conf.brightness*10);   
    text_display(&brightness_vol, bri_str);

    text_init(&brightness,&str_pos,font_size(SET_OPTION_INFO));
    brightness.erase = true;
    brightness.align = LEFT_BOTTOM;    
    brightness.offset.x = language_get() == language_arabic ? 26 : 0;
    language_text_display(&brightness, text_str(STR_BRIGHTNESS),language_get());

    
    position pos = {{363, 192},
                    {600, 1}};
    draw_rect(&pos, 0x40ffffff);
}

static void manual_QR_display(void){
    icon manual_QR;
    position pos = {{905,387},{40,40}};
    resource res = resource_get(ROM_R_2_SET_MANUAL_QR_2_PNG);
    icon_init(&manual_QR,&pos,&res);
    manual_QR.erase = true;
    icon_display(&manual_QR);
}
static void QR_code_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int QR_button = SequenceSearch(QR_CODE_BUTTON, (Par_Button),QR_BUTTON_TOTAL);
    switch (QR_button)
    {
        case TUYA_QR:
            gui_erase(&Par_Button->pos, 0X00);
            button_disable(Par_Button);
            button_control_group(OTHER_SET,MANUAL_DOWNLOAD,is_sdcard_insert() ? CCTV1_SET:UPGRADE_OUTDOOR,button_start_using);
            manual_QR_display();
            break;
        case OFFICIAL_QR1:
            button_start_disable(Par_Button);
            is_open_QR1 = false;
            button_start_using(&QR_CODE_BUTTON[OFFICIAL_QR2]);
            button_start_using(&tuya_app_button);       
            if(prev_time_date == &SYSTEM_TIME[DATA_BUTTON]){
                general_date_display();
                button_start_using(&GENERAL_ADJUST[UP_Y_BUTTON]);                
            }

            break;
        case OFFICIAL_QR2:
            button_start_disable(Par_Button);
            button_start_using(&QR_CODE_BUTTON[OFFICIAL_QR1]);
            is_open_QR1 = true;
            if(prev_time_date == &SYSTEM_TIME[DATA_BUTTON])
                button_disable(&GENERAL_ADJUST[UP_Y_BUTTON]);
                button_disable(&tuya_app_button);
            break;    
        default:
            break;
    }
}
static void QR_code_button_init(enum QR_CODE QR ,position pos,resource res)
{
    button_init(&QR_CODE_BUTTON[QR],&pos,QR_code_button_up);
    button_icon_init(&QR_CODE_BUTTON[QR],&res);
    QR_CODE_BUTTON[QR].icon_align = CENTER_BOTTOM;
}
static void QR_code_button_list(void){
    position pos_tuya = {{785,380},{174,174}};
    resource res_tuya = resource_get(ROM_R_2_SET_MANUAL_QR_PNG);
    QR_code_button_init(TUYA_QR,pos_tuya,res_tuya);

    position pos_official_QR2 = {{848,164},{40,40}};
    resource res_official_QR2 = resource_get(ROM_R_2_SET_MANUAL_QR_2_PNG);
    QR_code_button_init(OFFICIAL_QR2,pos_official_QR2,res_official_QR2);

    position pos_official_QR1 = {{733,153},{174,174}};
    resource res_official_QR1 = resource_get(ROM_R_2_SET_OFFICIAL_QR1_PNG);
    QR_code_button_init(OFFICIAL_QR1,pos_official_QR1,res_official_QR1);
}

static void other_button_up(const void *arg)
{
    int other_button = SequenceSearch(OTHER_SET, (Par_Button),OTHER_SET_TOTAL);

    if(other_button != BRAND_SET && other_button != BRIGHTNESS_SUB && other_button != BRIGHTNESS_ADD && other_button != MANUAL_DOWNLOAD)
        erase_bg();        

    DEBUG_LOG("--------------->>>%s     >>>>:%d\n\r",__func__,other_button);
   switch (other_button)
    {
        case BRIGHTNESS_SUB:
            DEBUG_LOG("--------------->>>BRIGHTNESS_SUB \n\r");
            if(user_data_get()->user_default_mode_conf.brightness > 1)
                user_data_get()->user_default_mode_conf.brightness --;
            set_lcd_brightness(user_data_get()->user_default_mode_conf.brightness);
            other_dajust_text_display();
            break;
        case BRIGHTNESS_ADD:
            DEBUG_LOG("--------------->>>BRIGHTNESS_ADD \n\r");
            if(user_data_get()->user_default_mode_conf.brightness < 10)
                user_data_get()->user_default_mode_conf.brightness ++;
            set_lcd_brightness(user_data_get()->user_default_mode_conf.brightness);
            other_dajust_text_display();
            break;
        case SET_PASSWORD:
            DEBUG_LOG("--------------->>>SET_PASSWORD \n\r");
            prev_lay = Par_Button;
            confirmation_window_display(209,401,134,STR_SET_PASSWORD,false);
            button_disable(&QR_CODE_BUTTON[TUYA_QR]);
            button_control_group(OTHER_SET,BRIGHTNESS_SUB,CCTV1_SET,button_disable);
            button_control_group(OPTION_ACK,OTHER_YES_BUTTON,CONNECT_YES_BUTTON,button_start_using);
            button_control_group(INPUT_BUT,OTHER_OLD_PASSWORD,CONNECT_WIFI_NAME,button_start_using);
            button_control_group(INPUT_SHOW_BUT,OTHER_OLD_PASSW,CONNECT_WIFI_PASSW,button_start_using);
            // button_start_using(&RETURN_BUTTON[PASSWORD_RETURN]);
            input_subtitle_diaplay(STR_OLD_PASSWORD,STR_NEW_PASSWORD);
            // window_headline_diaplay(STR_SET_PASSWORD);

            if(input_buffer[ALARM_OLD_PASS_BOX] == NULL)  //旧密码未填入则新密码无法修改
                button_disable(&INPUT_BUT[OTHER_NEW_PASSWORD]);

            password_display(&input_buffer[ALARM_OLD_PASS_BOX]);
            password_display(&input_buffer[ALARM_NEW_PASS_BOX]);
            break;
        case DEVICE_SET:
            DEBUG_LOG("--------------->>>DEVICE_SET \n\r");
            cur_cctv = NULL;
            button_control_group(OTHER_SET,BRIGHTNESS_SUB,CCTV1_SET,button_disable);
            button_disable(&QR_CODE_BUTTON[TUYA_QR]);
            button_control_group(OTHER_SET,CCTV1_SET,BRAND_SET,button_start_using);
            button_start_using(&RETURN_BUTTON[DEVICE_RETURN]);
            window_headline_diaplay(STR_SET_DEVICE);
            break;

        case DEVICE_INFORMATION:
            DEBUG_LOG("--------------->>>DEVICE_INFORMATION \n\r");
            cur_cctv = NULL;
            button_control_group(OTHER_SET,BRIGHTNESS_SUB,CCTV1_SET,button_disable);
            button_disable(&QR_CODE_BUTTON[TUYA_QR]);
            button_start_using(&RETURN_BUTTON[DEVICE_RETURN]);
            window_headline_diaplay(STR_DEVICE_INFO);
            system_info_display();
            break;

        case MANUAL_DOWNLOAD:
            DEBUG_LOG("--------------->>>MANUAL_DOWNLOAD \n\r");
            button_start_using(&QR_CODE_BUTTON[TUYA_QR]);
            button_control_group(OTHER_SET,MANUAL_DOWNLOAD,is_sdcard_insert() ? CCTV1_SET:UPGRADE_OUTDOOR,button_disable);
            break;

        case UPGRADE_OUTDOOR:
            DEBUG_LOG("--------------->>>UPGRADE_OUTDOOR \n\r");
            button_control_group(OTHER_SET,BRIGHTNESS_SUB,CCTV1_SET,button_disable);
            button_control_group(OTHER_UPGRADE,UPGRADE_OUTDOOR1,OTHER_UPGRADE_TOTAL,button_start_using);
            button_start_using(&RETURN_BUTTON[UPGRADE_RETURN]);
            window_headline_diaplay(STR_UPGRADE_OUTDOOR);
            break;

        case CCTV1_SET:
            prev_lay = Par_Button;
            cur_cctv = &user_data_get()->ctv_info.CCTV1;
            window_headline_diaplay(STR_CCTV1);
            button_control_group(OTHER_SET,CCTV1_SET,BRAND_SET,button_disable);
            button_control_group(OTHER_SET,BRAND_SET,OTHER_SET_TOTAL,button_start_using);
            button_disable(&RETURN_BUTTON[DEVICE_RETURN]);
            button_start_using(&RETURN_BUTTON[CCTV_RETURN]);
            cctv_info_display(cur_cctv,true);
            break;
        case CCTV2_SET:              
            prev_lay = Par_Button;
            cur_cctv = &user_data_get()->ctv_info.CCTV2;
            window_headline_diaplay(STR_CCTV2);
            button_control_group(OTHER_SET,CCTV1_SET,BRAND_SET,button_disable);
            button_control_group(OTHER_SET,BRAND_SET,OTHER_SET_TOTAL,button_start_using);
            button_disable(&RETURN_BUTTON[DEVICE_RETURN]);
            button_start_using(&RETURN_BUTTON[CCTV_RETURN]);
            cctv_info_display(cur_cctv,true);
            break;
        case BRAND_SET:
            if(cur_cctv ==  &user_data_get()->ctv_info.CCTV1){
                user_data_get()->ctv_info.CCTV1.CCTV_BRAND = !user_data_get()->ctv_info.CCTV1.CCTV_BRAND;
            }else{
                user_data_get()->ctv_info.CCTV2.CCTV_BRAND = !user_data_get()->ctv_info.CCTV2.CCTV_BRAND;
            }
            cctv_info_display(cur_cctv,true);
            break;
        case IP_SET:
        case ACCOUNT:
        case CCTV_PASSWORD:
            curr_input_box = ((&OTHER_SET[CCTV1_SET] == prev_lay)?CCTV1_IP_BOX:CCTV2_IP_BOX ) + other_button - IP_SET;
            DEBUG_LOG("INPUT_CCTV  BOX>>>>>>>>:%d \n\r",curr_input_box);
            app_layout_goto(&layout_key_board);
            break;
        default:
            break;
    }
}
static void other_button_down(const void *arg)
{
    if(Par_Button == &OTHER_SET[BRIGHTNESS_SUB]){
        button_refresh(Par_Button,ROM_R_2_SET_ADJUST_SUB_SELECT_PNG,ROM_R_2_SET_ADJUST_SUB_SELECT_PNG_SIZE,true);
    }else if(Par_Button == &OTHER_SET[BRIGHTNESS_ADD]){
        button_refresh(Par_Button,ROM_R_2_SET_ADJUST_ADD_SELECT_PNG,ROM_R_2_SET_ADJUST_ADD_SELECT_PNG_SIZE,true); 
    }else if(Par_Button == &OTHER_SET[MANUAL_DOWNLOAD]){
        button_refresh(Par_Button,ROM_R_2_SET_OTHER_BUTTON_ON_PNG,ROM_R_2_SET_OTHER_BUTTON_ON_PNG_SIZE,true);
        manual_QR_display();
    }else{
        button_refresh(Par_Button,ROM_R_2_SET_OTHER_BUTTON_ON_PNG,ROM_R_2_SET_OTHER_BUTTON_ON_PNG_SIZE,true);
    }
    if(cur_cctv != NULL)
        cctv_info_display(cur_cctv,false);
}
static void other_button_leave_down(const void *arg)
{
    if(Par_Button == &OTHER_SET[BRIGHTNESS_SUB]){
        button_refresh(Par_Button,ROM_R_2_SET_ADJUST_SUB_PNG,ROM_R_2_SET_ADJUST_SUB_SELECT_PNG_SIZE,true);
    }else if(Par_Button == &OTHER_SET[BRIGHTNESS_ADD]){
        button_refresh(Par_Button,ROM_R_2_SET_ADJUST_ADD_PNG,ROM_R_2_SET_ADJUST_ADD_PNG,true); 
    }else if(Par_Button == &OTHER_SET[MANUAL_DOWNLOAD]){
        button_refresh(Par_Button,ROM_R_2_SET_OTHER_BUTTON_PNG,ROM_R_2_SET_OTHER_BUTTON_PNG_SIZE,true);
        manual_QR_display();
    }else{
        button_refresh(Par_Button,ROM_R_2_SET_OTHER_BUTTON_PNG,ROM_R_2_SET_OTHER_BUTTON_PNG_SIZE,true);
    }
    if(cur_cctv != NULL)
        cctv_info_display(cur_cctv,false);
}
static void other_button_init(button *Button,position pos,resource res,enum btn_string_id id)
 {
    button_init(Button,&pos,other_button_up);
    button_icon_init(Button,&res);
    button_text_init(Button,btn_str(id),font_size(SET_OPTION_INFO));
    Button->down = other_button_down;
    Button->leave_down = other_button_leave_down;
    Button->font_align = LEFT_MIDDLE;
    Button->font_offset.x = 20;
 }
 static void other_adjust_button_init(button *Button,position pos,resource res)
 {
    button_init(Button,&pos,other_button_up);
    button_icon_init(Button,&res);
    Button->down = other_button_down;
    Button->leave_down = other_button_leave_down;
 }

static void other_main_button_offset(void)
 {
    OTHER_SET[DEVICE_SET].font_offset.x  = language_get() == language_arabic ? 40 : 20;
    OTHER_SET[DEVICE_INFORMATION].font_offset.x  = language_get() == language_arabic ? 25 : 20;
    OTHER_SET[MANUAL_DOWNLOAD].font_offset.x  = language_get() == language_arabic ? 45 : 20;
    OTHER_SET[UPGRADE_OUTDOOR].font_offset.x  = language_get() == language_arabic ? 45 : 20;
 }
static void other_main_button_list(void)
{
    position pos_s = {{700,123},{70,60}};
    position pos = {{363,200},{600,60}};
    resource res_s = resource_get(ROM_R_2_SET_ADJUST_SUB_PNG);
    resource res_a = resource_get(ROM_R_2_SET_ADJUST_ADD_PNG);
    resource  res = resource_get(ROM_R_2_SET_OTHER_BUTTON_PNG);

    other_adjust_button_init(&OTHER_SET[BRIGHTNESS_SUB],pos_s,res_s);
    pos_s.point.x = 870;
    other_adjust_button_init(&OTHER_SET[BRIGHTNESS_ADD],pos_s,res_a);

    other_button_init(&OTHER_SET[SET_PASSWORD],pos,res,STR_SET_PASSWORD);
    pos.point.y += 60;
    other_button_init(&OTHER_SET[DEVICE_SET],pos,res,STR_SET_DEVICE);
    pos.point.y += 60;
    other_button_init(&OTHER_SET[DEVICE_INFORMATION],pos,res,STR_DEVICE_INFO);
    pos.point.y += 60;
    other_button_init(&OTHER_SET[MANUAL_DOWNLOAD],pos,res,STR_MANUAL);
    pos.point.y += 60;
    other_button_init(&OTHER_SET[UPGRADE_OUTDOOR],pos,res,STR_UPGRADE_OUTDOOR);

    position cctv_pos = {{356,168},{600,60}};
    other_button_init(&OTHER_SET[CCTV1_SET],cctv_pos,res,STR_CCTV1);    
    cctv_pos.point.y += 60;
    other_button_init(&OTHER_SET[CCTV2_SET],cctv_pos,res,STR_CCTV2);

    position set_c_pos = {{356,158},{600,60}};    
    other_button_init(&OTHER_SET[BRAND_SET],set_c_pos,res,STR_BRAND);    
    set_c_pos.point.y += 60;
    other_button_init(&OTHER_SET[IP_SET],set_c_pos,res,STR_IP);
    set_c_pos.point.y += 60;
    other_button_init(&OTHER_SET[ACCOUNT],set_c_pos,res,STR_ACCOUNT);
    set_c_pos.point.y += 60;
    other_button_init(&OTHER_SET[CCTV_PASSWORD],set_c_pos,res,STR_PASSWORD);
}

static int net_send_target;
static void other_upgrade_up(const void *arg)
{
    net_send_target = (Par_Button == &OTHER_UPGRADE[UPGRADE_OUTDOOR1]) ? DEVICE_OUTDOOR_1 : DEVICE_OUTDOOR_2;
    if(server_update_file_exist() && device_list_ip_get(net_send_target) != NULL){
        create_upgrade_server_task();
    }else{
        DEBUG_LOG("server update file not exist or camera be not online !!!!!!!!!!!!!!!!! \n\r");
        DEBUG_LOG("device_list_ip_get(%d) !!!!!!!!!!!!!!!!! %s\n\r",net_send_target,device_list_ip_get(net_send_target));
        DEBUG_LOG("server_update_file_exist : %d !!!!!!!!!!!!!!!!! \n\r",server_update_file_exist());
        button_refresh(Par_Button,ROM_R_2_SET_CAMERA_OFF_PNG,ROM_R_2_SET_CAMERA_OFF_PNG_SIZE,true);
        return;
    }
    net_common_data_send(net_send_target,NET_CMD_APP_UPGRADE,(unsigned char *)device_list_ip_get(user_data_get()->user_default_device),strlen((const char*)device_list_ip_get(user_data_get()->user_default_device))+1);
    network_data_param_set(net_send_target,NET_CMD_APP_UPGRADE,(unsigned char *)device_list_ip_get(user_data_get()->user_default_device),strlen((const char*)device_list_ip_get(user_data_get()->user_default_device))+1,2);
    button_control_group(OTHER_UPGRADE,UPGRADE_OUTDOOR1,OTHER_UPGRADE_TOTAL,button_disable);
}
static void other_upgrade_down(const void *arg)
{
    button_refresh(Par_Button,ROM_R_2_SET_CAMERA_ON_PNG,ROM_R_2_SET_CAMERA_ON_PNG_SIZE,true);
}
// static void other_upgrade_leave_down(const void *arg)
// {
//     button_refresh(Par_Button,ROM_R_2_SET_CAMERA_OFF_PNG,ROM_R_2_SET_CAMERA_OFF_PNG_SIZE,true);
// }
static void other_upgrade_button_init(button *Button,position pos,resource res,enum btn_string_id id)
 {
    button_init(Button,&pos,other_upgrade_up);
    button_icon_init(Button,&res);
    button_text_init(Button,btn_str(id),font_size(SET_OPTION_INFO));
    Button->down = other_upgrade_down;
    // Button->leave_down = other_upgrade_leave_down;
    Button->font_align = CENTER_BOTTOM;
 }
static void other_upgrade_button_list(void)
{
    position pos = {{478,224},{139,200}};
    resource  res = resource_get(ROM_R_2_SET_CAMERA_OFF_PNG);
    other_upgrade_button_init(&OTHER_UPGRADE[UPGRADE_OUTDOOR1],pos,res,STR_CAM1);
    pos.point.x = 741;
    other_upgrade_button_init(&OTHER_UPGRADE[UPGRADE_OUTDOOR2],pos,res,STR_CAM2);
}




static void other_interface_display(void)
{
    other_dajust_text_display();
    other_main_button_offset();
    button_control_group(OTHER_SET,BRIGHTNESS_SUB,is_sdcard_insert() ? CCTV1_SET : UPGRADE_OUTDOOR,button_start_using);
    manual_QR_display();
}

static void akc_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int ack_but = SequenceSearch(OPTION_ACK, Par_Button,OPTION_ACK_TOTAL);
    DEBUG_LOG("ack_but------------->>>%d \n\r",ack_but);
    switch (ack_but)
    {
    case RESET_YES_BUTTON:
        confirmation_window_display(242,401,172,STR_WAIT_ISSUE,true);
        Pop_Operation_Tips = true;
        break;
    case RESET_NO_BUTTON:
        app_layout_goto(&layout_home);
        break;
    case OTHER_YES_BUTTON:
        if(input_buffer[ALARM_NEW_PASS_BOX] != NULL)
        {
            DEBUG_LOG("=================>>OTHER_YES_BUTTON\n\r");
            Password_updated();
            button_listens_destroy();
            Pop_Operation_Tips = true;       
            Operation_Tips =  STR_MODIFY_SUCCESSFULLY;
            network_data_param_set(DEVICE_ALL,NET_CMD_ALARM_SYNC_PASSWORD,(unsigned char *)user_data_get()->alarm_pass.old_pass,strlen((const char*)user_data_get()->alarm_pass.old_pass)+1,3);
        }

        break;
    case OTHER_NO_BUTTON:
        free_pointer(&input_buffer[ALARM_OLD_PASS_BOX]);//退出输入密码窗口即释放密码输入buffer
        free_pointer(&input_buffer[ALARM_NEW_PASS_BOX]);
        set_button_up(curr_main_but); 
        break;   
    case NET_TIME_ACK_BUTTON:
        Pop_Confirm_Tips = false;
        hint_window_diaplay(STR_TUYA_NET_TIME_TEXT1,STR_TUYA_NET_TIME_TEXT2,false);
        // enable_prev_lay_button(backup_button_group,&backup_button_total);
        button_disable(&OPTION_ACK[NET_TIME_ACK_BUTTON]);
        button_start_using(&goto_home_button);
        button_control_group(MAIN_SET,SOUND_BUTTON,MAIN_SET_BUTTON_TOTAL,button_start_using);
        if(tuya_get_app_register_status() == E_IPC_ACTIVEATED){
            get_network_time();
            ak_get_localdate(&user_data_get()->user_default_time);
            // general_time_display();
        }
        reset_cut_off_display();
        set_button_up(curr_main_but); 
        break;   
        
    case TUYA_CTRL_ACK_BUTTON:{
        extern bool tuya_sdk_init_status();
        Pop_Confirm_Tips = false;
        if(tuya_sdk_init_status()){
            user_data_get()->tuya_open = false;
            Save_config(FACTORY_CONF_FILE_PATH);
            amp_enable(false);
            system("reboot");
        }
        else
        {
            confirm_interface(STR_TOTAL,STR_TOTAL,true,VERY_TRANSPARENT,font_size(SET_OPTION_INFO));
            enable_prev_lay_button(backup_button_group,&backup_button_total);
            reset_cut_off_display();
            set_button_up(curr_main_but); 
        }
        break;   
    }
    case TUYA_CTRL_NO_BUTTON:
        Pop_Confirm_Tips = false;
        confirm_interface(STR_TOTAL,STR_TOTAL,true,VERY_TRANSPARENT,font_size(SET_OPTION_INFO));
        enable_prev_lay_button(backup_button_group,&backup_button_total);
        reset_cut_off_display();
        set_button_up(curr_main_but); 
        break;
    default:
        break;
    }
}
static void akc_button_down(const void *arg)
{
    if(Par_Button != &OPTION_ACK[NET_TIME_ACK_BUTTON])
    button_refresh(Par_Button,ROM_R_2_SET_ACK_BUTTON_ON_PNG,ROM_R_2_SET_ACK_BUTTON_ON_PNG_SIZE,true);
}
static void akc_button_leave_down(const void *arg)
{
    if(Par_Button != &OPTION_ACK[NET_TIME_ACK_BUTTON])
    button_refresh(Par_Button,ROM_R_2_SET_ACK_BUTTON_PNG,ROM_R_2_SET_ACK_BUTTON_PNG_SIZE,true);
}
static void akc_button_init(button *Button,position pos,enum btn_string_id id,bool is_YES)
{
    resource res_yes = resource_get(ROM_R_2_SET_ACK_BUTTON_PNG);
    resource res_no = resource_get(ROM_R_2_SET_ACK_BUTTON_PNG);
    button_init(Button,&pos,akc_button_up);
    button_icon_init(Button,is_YES ? &res_yes : &res_no);
    button_text_init(Button,btn_str(id),font_size(SET_RESET_OPTION));
    Button->font_color = WHITE;
    Button->font_align = CENTER_MIDDLE;
    Button->down = akc_button_down;
    Button->leave_down = akc_button_leave_down;
}
static void akc_button_list(void)
{
    position pos = {{420,293},{210,60}};
    akc_button_init(&OPTION_ACK[RESET_YES_BUTTON],pos,STR_YES,true);
    OPTION_ACK[RESET_YES_BUTTON].font_color = RED;
    pos.point.x = 689;
    akc_button_init(&OPTION_ACK[RESET_NO_BUTTON],pos,STR_NO,false);

    position pas = {{417,451},{210,60}};
    akc_button_init(&OPTION_ACK[OTHER_YES_BUTTON],pas,STR_APPROVAL,true);
    OPTION_ACK[OTHER_YES_BUTTON].font_color = RED;

    akc_button_init(&OPTION_ACK[CONNECT_YES_BUTTON],pas,STR_APPROVAL,true);

    pas.point.x = 685;
    akc_button_init(&OPTION_ACK[OTHER_NO_BUTTON],pas,STR_BACK,false);    
    
    akc_button_init(&OPTION_ACK[CONNECT_NO_BUTTON],pas,STR_BACK,false);    

    position pac = {{225,396},{575,80}};
    akc_button_init(&OPTION_ACK[NET_TIME_ACK_BUTTON],pac,STR_YES,true); 

    position pas_tuya = {{274,341},{210,60}};   
    akc_button_init(&OPTION_ACK[TUYA_CTRL_ACK_BUTTON],pas_tuya,STR_YES,true); 
    pas_tuya.point.x = 542;
    akc_button_init(&OPTION_ACK[TUYA_CTRL_NO_BUTTON],pas_tuya,STR_NO,false); 
}

static void other_all_button_list(void)
{
    QR_code_button_list();
    other_main_button_list();
    other_upgrade_button_list();
    input_button_list();
    akc_button_list();
}
#endif

#ifdef MAIN_SET_PAGE
static void set_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    button_enable(curr_main_but); //切换其他主要按键时使能之前的按键解封
    int prev_but = SequenceSearch(MAIN_SET, curr_main_but,MAIN_SET_BUTTON_TOTAL);
    int main_set_but = SequenceSearch(MAIN_SET, Par_Button,MAIN_SET_BUTTON_TOTAL);
    curr_main_but->font_color = GRAY;

        

        switch(prev_but)
        {
            case GENERAL_BUTTON:
                if(prev_but != main_set_but)
                {
                    button_control_group(DEVICE,DEVICE_1_BUTTON,DEVICE_BUTTON_TOTAL,button_disable);
                    button_control_group(LANGUAGE,LANGUAGE_E_BUTTON,LANGUAGE_BUTTON_TOTAL,button_disable);
                    button_control_group(GENERAL_ADJUST,UP_D_BUTTON,DOWN_MI_BUTTON+1,button_disable);
                    button_control_group(QR_CODE_BUTTON,OFFICIAL_QR1,QR_BUTTON_TOTAL,button_disable);
                    button_control_group(SYSTEM_TIME,TIME_BUTTON,SYS_TIME_TOTAL,button_disable);
                    button_disable(&tuya_app_button);
                }
                break;
            case SOUND_BUTTON:
                if(prev_but != main_set_but)
                {
                    button_disable(&SOUND_SOURCE[LEFT_SWITCH_BUTTON]);
                    button_disable(&SOUND_SOURCE[RIGHT_SWITCH_BUTTON]);
                    button_control_group(SOUND_ADJUST,CALL_VOL_SUB,SOUND_ADJUST_TOTAL,button_disable);
                }
                break;
    #ifdef Set_Connect 
            case CONNECT_BUTTON:
                button_disable(&SWITCH_BUT[WIFI_POWER]);
                button_disable(&manually_button);
                button_disable(&RETURN_BUTTON[CONNECT_RETURN]);
                button_control_group(OPTION_ACK,CONNECT_YES_BUTTON,NET_TIME_ACK_BUTTON,button_disable);    
                button_control_group(INPUT_BUT,CONNECT_WIFI_NAME,INPUT_TYPE_TOTAL,button_disable);    
                button_disable(&INPUT_SHOW_BUT[CONNECT_WIFI_PASSW]);
                break;
    #endif
            case MODE_BUTTON:
                if(prev_but != main_set_but)
                {
                    button_control_group(SWITCH_BUT,ALWAYS_NO,WIFI_POWER,button_disable);
                    button_control_group(MODE_ADJUST,BRIGHTNESS_SUB,MODE_ADJUST_TOTAL,button_disable);
                }
                break;
    #ifdef Set_Other
            case OTHER_BUTTON:
                button_disable(&QR_CODE_BUTTON[TUYA_QR]);
                button_control_group(OTHER_SET,BRIGHTNESS_SUB,OTHER_SET_TOTAL,button_disable);
                button_control_group(OPTION_ACK,OTHER_YES_BUTTON,CONNECT_YES_BUTTON,button_disable);   
                button_control_group(INPUT_BUT,OTHER_OLD_PASSWORD,CONNECT_WIFI_NAME,button_disable);   
                button_control_group(INPUT_SHOW_BUT,OTHER_OLD_PASSW,CONNECT_WIFI_PASSW,button_disable); 
                button_control_group(RETURN_BUTTON,PASSWORD_RETURN,CCTV_RETURN,button_disable);          
                button_control_group(OTHER_UPGRADE,UPGRADE_OUTDOOR1,OTHER_UPGRADE_TOTAL,button_disable);
                break;
    #endif
            case RESET_BUTTON:
                if(prev_but != main_set_but)
                {           
                    curr_main_but->font_color = RED;       
                    button_disable(&OPTION_ACK[RESET_YES_BUTTON]);
                    button_disable(&OPTION_ACK[RESET_NO_BUTTON]);
                    button_disable(&INPUT_SHOW_BUT[CONNECT_WIFI_PASSW]);
                }
                break;
        }        


    erase_bg();
    button_display(curr_main_but);      
    curr_main_but =  (Par_Button);
    (Par_Button)->font_color = WHITE;

    switch (main_set_but)
    {
        case GENERAL_BUTTON:
            DEBUG_LOG("--------------->>>GENERAL_BUTTON \n\r");
            general_interface_display(); 
            break;
        case SOUND_BUTTON:
            DEBUG_LOG("--------------->>>SOUND_BUTTON \n\r");
            sound_interface_display();
            break;
#ifdef Set_Connect 
        case CONNECT_BUTTON:
            DEBUG_LOG("--------------->>>CONNECT_BUTTON \n\r");
            prev_lay = arg;
            button_start_using(&SWITCH_BUT[WIFI_POWER]);

            wifi_head_display();
            if(is_open_wifi)
            {
                button_start_using(&manually_button);                
                manually_text_display();                
            }
            break;
#endif
        case MODE_BUTTON:
            DEBUG_LOG("--------------->>>mode_button \n\r");
            mode_interface_display();
            button_control_group(SWITCH_BUT,ALWAYS_NO,WIFI_POWER,button_start_using);     
            break;
#ifdef Set_Other
        case OTHER_BUTTON:
            DEBUG_LOG("--------------->>>OTHER_BUTTON \n\r");
            cur_cctv = NULL; 
            other_interface_display();
            break;
#endif
        case RESET_BUTTON:
            DEBUG_LOG("--------------->>>RESET_BUTTON \n\r");

            confirmation_window_display(242,401,172,STR_ISSUE,false);
            button_start_using(&OPTION_ACK[RESET_YES_BUTTON]);
            button_start_using(&OPTION_ACK[RESET_NO_BUTTON]);
            (Par_Button)->font_color = RED;
            break;

        default:
            break;
    }
    button_disable(Par_Button); //禁用当前主要按键，防止二次进入
    button_display(Par_Button);
}

static void main_set_button_init(button *Button,position pos,void *string)
{
    button_init(Button,&pos,set_button_up);
    button_text_init(Button,string,font_size(SET_OPTION));
    Button->font_color = GRAY;
    Button->erase = true;
    Button->font_align = LEFT_MIDDLE;   
    // Button->bg_color = BLUE;
}

static void main_set_button_list(void)
{
    position pos = {{90,100},{162,80}};
    main_set_button_init(&MAIN_SET[GENERAL_BUTTON],pos,btn_str(STR_GENERAL));

#ifndef Set_Connect
    pos.point.y = 200;
#else
    pos.point.y = 185;
#endif
    main_set_button_init(&MAIN_SET[SOUND_BUTTON],pos,btn_str(STR_SOUND));
#ifdef Set_Connect 
    pos.point.y = 265;
    main_set_button_init(&MAIN_SET[CONNECT_BUTTON],pos,btn_str(STR_CONNECT));
#endif

#ifndef Set_Connect
    pos.point.y = 300;
#else
    pos.point.y = 345;
#endif
    main_set_button_init(&MAIN_SET[MODE_BUTTON],pos,btn_str(STR_MODE));

#ifdef Set_Other
    pos.point.y = 400;
    main_set_button_init(&MAIN_SET[OTHER_BUTTON],pos,btn_str(STR_OTHER));

#endif
    pos.point.y = 512;
    main_set_button_init(&MAIN_SET[RESET_BUTTON],pos,btn_str(STR_RESET));
    MAIN_SET[RESET_BUTTON].font_color = RED;
}


#endif

#ifdef MAINFUNC

static void set_button_display(void)
{
           
    for(enum main_set_button but= GENERAL_BUTTON;but < MAIN_SET_BUTTON_TOTAL;but ++)
    {
        MAIN_SET[but].font_align = language_get() == language_arabic ? RIGHT_MIDDLE : LEFT_MIDDLE;
        MAIN_SET[but].font_offset.x = language_get() == language_arabic ? -12 : 35;
        MAIN_SET[but].font_color = but == RESET_BUTTON ? RED : GRAY;
    }
    goto_home_button_display(true);
    curr_main_but->font_color = WHITE;        
    button_control_group(MAIN_SET,GENERAL_BUTTON,MAIN_SET_BUTTON_TOTAL,button_display);
}

static void set_button_enable(void)
{
    button_enable(&goto_home_button);
    button_control_group(MAIN_SET,GENERAL_BUTTON,MAIN_SET_BUTTON_TOTAL,button_enable);
}

static void layout_set_init(void)
{
    //DEBUG_LOG("--------------->>>%s \n\r",__func__);
    prev_device = &DEVICE[user_data_get()->user_default_device];
    prev_language = &LANGUAGE[user_data_get()->user_default_language];
    goto_home_button_init();
    main_set_button_list();
    general_interface_list();
    sound_interface_list();
#ifdef Set_Connect 
    connect_button_list();
#endif
    mode_interface_list();
#ifdef Set_Other
    other_all_button_list();
    return_button_list();
    input_show_button_list();         
#endif    
    
}


static void layout_set_enter(void)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    // DEBUG_LOG("%d==================>>%lld\n\r",__LINE__,os_get_ms());
    // int main_set_but = SequenceSearch(MAIN_SET, curr_main_but,MAIN_SET_BUTTON_TOTAL);
    ak_get_localdate(&user_data_get()->user_default_time);
    set_button_enable();   
    // DEBUG_LOG("main_set_but-------------->>>%d\n\r",main_set_but); 


    if(prev_layout != &layout_key_board)
    {
        curr_main_but = &MAIN_SET[GENERAL_BUTTON];    
        // DEBUG_LOG("--------------->>>GENERAL_BUTTON \n\r");
        general_interface_display();    //需优化
        
        free_pointer(&input_buffer[ALARM_OLD_PASS_BOX]);
        free_pointer(&input_buffer[ALARM_NEW_PASS_BOX]); 
    }
    else
    {
#ifdef Set_Other         
        if(curr_main_but == &MAIN_SET[OTHER_BUTTON])
        {
            other_button_up(prev_lay);
        }
#endif
#ifdef Set_Connect 
        else if(curr_main_but == &MAIN_SET[CONNECT_BUTTON])
        {
            add_manually_button_up(prev_lay);
        }
#endif
    }
    button_disable(curr_main_but);//禁用当前主要按键，防止二次进入
    set_button_display();
    menu_back_ground_color();
    
    reset_cut_off_display();   
    // DEBUG_LOG("%d==================>>%lld\n\r",__LINE__,os_get_ms());
}

static void layout_set_quit(void)
{
    // DEBUG_LOG("--------------->>>%s           %lld\n\r",__func__,os_get_ms());
    // destroy_upgrade_server_task();
    png_buffer_destroy();
    prev_time_date= &SYSTEM_TIME[DATA_BUTTON];

    if(time_corrected == true){
        if(user_data_get()->user_default_mode_conf.user_default_net_time && tuya_get_app_register_status() == E_IPC_ACTIVEATED){
            get_network_time();
        }else{
            date_set(&user_data_get()->user_default_time);
        }
        time_corrected = false;
    }

    if(unlock_time_modify == true){
        unlock_time_modify = false;
        net_common_send(DEVICE_ALL,NET_CMD_UNLOCK_TIME_SYNC,user_data_get()->user_default_mode_conf.time_conf.door_time,0);
        looup_time_set_param(DEVICE_ALL,NET_CMD_UNLOCK_TIME_SYNC,3);
    }


    if(goto_layout == &layout_key_board)
    {
        if(curr_input_box == ALARM_OLD_PASS_BOX)
        free_pointer(&input_buffer[ALARM_OLD_PASS_BOX]);
        else if(curr_input_box == ALARM_NEW_PASS_BOX)
        free_pointer(&input_buffer[ALARM_NEW_PASS_BOX]);        
    }
    else
    {
        free_pointer(&input_buffer[ALARM_OLD_PASS_BOX]);
        free_pointer(&input_buffer[ALARM_NEW_PASS_BOX]);        
    }
    Save_config(FACTORY_CONF_FILE_PATH); 
    // DEBUG_LOG("--------------->>>%s           %lld\n\r",__func__,os_get_ms());
}

static void layout_set_timer(void)
{
    unsigned long  long pts = os_get_ms();   
    static unsigned long long pre_pts = 0;
    static unsigned long long time_300ms = 0;
    static unsigned long long time_3s = 0;
    static bool is_first = false;    
    if(!but_press_time&&play_melody)
    {
        play_melody = false;
        audio_play(user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_mel,
                                user_data_get()->SOUR_PRO[cur_sound_sour].sound.call_vol*TONE_VOLUME_INTERVAL+TONE_MIN_VOLUME ,
                                user_data_get()->SOUR_PRO[cur_sound_sour].sound.speak_sen,audio_start_callback,audio_finish_callback);     
    }
    else
    {
        but_press_time--;
    }

    if(((pts - time_300ms)>500))
    {
        time_300ms = pts;
        time_3s ++;
        if(time_3s  == 30){
            time_3s  = 0;
            Save_config(FACTORY_CONF_FILE_PATH);     
        }

        network_data_send_start();
        
        if(upgrade_outdoor_status() == false){
            // #if PRINTFS
            // DEBUG_LOG("[send  %d]>>>>>>>>>>>>>>>>>>>> 0x%x    param1 :%s                   param2 :%d\n\r",net_send_target,net_send_cmd,net_send_param1,net_send_param2);
            // #endif
        }else if(upgrade_outdoor_status() == true){
            set_upgrade_outdoor_status(false);
            button_control_group(OTHER_UPGRADE,UPGRADE_OUTDOOR1,OTHER_UPGRADE_TOTAL,button_start_using);
            button_refresh(net_send_target == DEVICE_OUTDOOR_1 ? &OTHER_UPGRADE[UPGRADE_OUTDOOR1] : &OTHER_UPGRADE[UPGRADE_OUTDOOR2],
            ROM_R_2_SET_CAMERA_OFF_PNG,ROM_R_2_SET_CAMERA_OFF_PNG_SIZE,true);
        }


        if((get_layout_switch_flag() || device_status_update()) && curr_main_but == &MAIN_SET[GENERAL_BUTTON]  && Pop_Confirm_Tips == false){

            // DEBUG_LOG("tuya_open_device_get() ==============>%d\n\r",tuya_open_device_get());
            general_tuya_resource_display();
            for(int i = 0;i<DEVICE_BUTTON_TOTAL;i++){
                if(i == user_data_get()->user_default_device) continue;

                if(device_list_ip_get(i) == NULL){
                    DEVICE[i].font_color = WHITE;
                    DEVICE[i].button_tone = KEY_1;
                }
                else{
                    DEVICE[i].font_color = BLUE;
                    DEVICE[i].button_tone = KEY_2;
                }
                button_display(&DEVICE[i]);
            }            
        }

    }
    if(Pop_Operation_Tips)
    {           
        if(pts - pre_pts > 1000 && Operation_Tips == STR_TOTAL){//这段必须放在前面，避免在复位时执行到hint_box_display引起段错误
            pre_pts = pts;
            if(!is_first){
                is_first = true;
            }else{
                amp_enable(false);
                set_lcd_brightness(0);
                user_data_restore();                        
            }
        }

        if((pts - pre_pts)>3000)
        {
            pre_pts = pts;
            if(!is_first)
            {
                is_first = true;
                hint_box_display(Operation_Tips,false);

            }
            else
            {
                is_first = false;
                Pop_Operation_Tips = false;
                Operation_Tips = STR_TOTAL;
                app_layout_goto(&layout_home);    
            }
        }
    }
}
layout layout_set = 
{
    .init = layout_set_init,
    .enter = layout_set_enter,
    .quit = layout_set_quit,
    .timer = layout_set_timer
};
#endif