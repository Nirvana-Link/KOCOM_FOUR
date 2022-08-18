#include <stdio.h>
#include <stdlib.h>
#include"layout_common.h"
#include"layout_home_button.h"
#include"l_absent_queue.h"
#include"l_audio_play.h"
#include"tuya_sdk_api.h"
#include"audio_play_api.h"
#include"tuya_sdk_api.h"
#include"debug.h"

button goto_home_button;

void hint_box_display(enum btn_string_id str_id,bool need_erasure)
{
    position pos = {{247,126},{530,300}};
    if(need_erasure)
    {
        gui_erase(&pos,0x00);
        return;
    }
    icon hint_box;    
    resource res = resource_get(ROM_R_8_KEYBOARD_HINT_BOX_PNG);
    icon_init(&hint_box,&pos,&res);
    icon_display(&hint_box);

    text hint_text;
    position text_pos = {{247,230},{530,60}};
    text_init(&hint_text,&text_pos,34);
    hint_box.align = CENTER_MIDDLE;
    hint_text.offset.y = 15;
    language_text_display(&hint_text,text_str(str_id),language_get());         
}
void alarm_sensor_irq(int ev,int arg1)
{
    switch (ev)
    {
    case NETWORK_EVENT_ALARM_CONTROL:
            DEBUG_LOG("NETWORK_EVENT_ALARM_CONTROL ===============>\n\r");
            user_data_get()->security.running = arg1 & 0x01;
            user_data_get()->security.Sensor1_open = arg1 & 0x02;
            user_data_get()->security.Sensor2_open = arg1 & 0x04;
            if(user_data_get()->security.running == false && security_in_alarm_state())
            {
                user_data_get()->security.dismissed = NONE_TRIGGER;
                user_data_get()->security.curr_trigger_id = NONE_TRIGGER;
                Audio_play_stop();
                app_layout_goto(&layout_home);
            }

        break;

    case NETWORK_EVENT_ALARM_TRIGGER:{
            // unsigned long long curr_absent_time = os_get_ms();
            int index = arg1 & 0x0F;
            network_device dev = arg1 >> 4;
            sensor_trigger_num is_sersor_dismissed = user_data_get()->security.dismissed & (1 << index);
            DEBUG_LOG("alarm_sensor_irq ->0x%x    device : %d    user_data_get()->security.dismissed: 0x%x\n\r",index,dev,user_data_get()->security.dismissed);
            if((is_sersor_dismissed == NONE_TRIGGER) && //该报警器未触发
                (user_data_get()->security.running == true) &&
                /* ((curr_absent_time - user_data_get()->security.absent_start_time) > (user_data_get()->user_default_mode_conf.user_default_absent_delay*1000)) && */
                (((index == 0)&&user_data_get()->security.Sensor1_open) ||
                ((index == 1)&&user_data_get()->security.Sensor2_open)))
            {
                user_data_get()->security.curr_trigger_id = index;
                user_data_get()->security.curr_trigger_device = dev;
                Save_config(FACTORY_CONF_FILE_PATH);     
                net_common_send(DEVICE_ALL,NET_CMD_ALARM_SYNC_ACK,user_data_get()->security.curr_trigger_id | (dev << 4),0);
                DEBUG_LOG("ALARM_SENSOR_IRQ ---------->0x%x  DEVICE:%d      DISMISSED: 0x%x\n\r",index,dev,user_data_get()->security.dismissed);
                app_layout_goto(&layout_alarm);
            }
        }
        break;

    case NETWORK_EVENT_ALARM_FREE:
        if(user_data_get()->security.dismissed & (1 << arg1)){//若当前该警报未解除
                DEBUG_LOG("NETWORK_EVENT_ALARM_FREE ===============>0x%d\n\r",user_data_get()->security.dismissed);
                net_common_send(DEVICE_ALL,NET_CMD_ALARM_FREE_ACK,user_data_get()->security.curr_trigger_id,0);
                user_data_get()->security.dismissed &= ~(1 << arg1);
                user_data_get()->security.curr_trigger_id = user_data_get()->security.dismissed != NONE_TRIGGER ? user_data_get()->security.dismissed - 1 : NONE_TRIGGER;
                Audio_play_stop();
                if(user_data_get()->security.dismissed == NONE_TRIGGER)
                {
                    app_layout_goto(&layout_home);
                }
                else
                {
                    app_layout_goto(&layout_alarm);
                }
        }
        break;
        
    default:
        break;
    }

}
#if SD_CARD
bool sd_curr_status = false;//sd卡状态标志  true：插入    false:拔出
/* 非CAPTURE界面作sd卡检测与图标显示功能，CAPTURE界面仅作检测，并返回检测结果 ，
    若界面切换后第一次调用该函数，即无需判断sd卡状态是否改变
    参数 layout_jump: 判断界面是否刚刚切换*/
bool delect_sdcard(bool layout_jump)
{
    bool is_sd_plug = is_sdcard_insert();
    /* 若sd卡状态发生改变且当前界面非CAPTURE，即更新sd卡当前状态 */
    if(((sd_curr_status != is_sd_plug)||layout_jump)&&(goto_layout != &layout_capture))
    {
        sd_curr_status = is_sd_plug;
        icon sd_card;
        resource res = resource_get(ROM_R_1_HOME_SD_NO_PNG);
        position pos = {{797,20},{32,40}};        
        if(is_sd_plug)
        {
            DEBUG_LOG("SD------------->> insertion\n\r");
            res.id = ROM_R_1_HOME_SD_PNG;
            res.size = ROM_R_1_HOME_SD_PNG_SIZE;     
        }
        else
        {
            DEBUG_LOG("SD------------->> pull out\n\r");            
        }

        if(goto_layout ==  &layout_door_cam)
        {
            pos.point.x = 20;
            pos.point.y =30;
        }
        icon_init(&sd_card,&pos,&res);
        icon_display(&sd_card);        
    }
    return is_sd_plug;
}
#endif
/* 判断是否闰年 */
bool is_leap_feb(struct ak_date date)
{
    if(date.month == FEB)
    {
        if(date.year%4)
            return false;
        else
            return true;
    }
    return false;
}
int SequenceSearch(button *but, button *Button , int total)
{
    int i;
    for(i=0; i<total; i++)
        if(Button== &but[i] )
            return i;
    return -1;
}

bool button_start_using(const button *Button)
{
    button_display((button *)Button);
    button_enable((button *)Button);
    return true;
}
bool button_start_disable(const button *Button)
{
    gui_erase(&Button->pos,0x00);
    button_disable((button *)Button);
    return true;
}
void button_refresh(button *Button,long int res,long int res_size,bool is_erase)
{   
    if(is_erase == true)
        gui_erase(&Button->pos,0x00);
    // ((button *)Button)->erase = true;
    if(res != 0)
    {
        Button->resource.id = res;
        Button->resource.size = res_size;        
    }
    //button_icon_init(Button,&Button->resource);
    button_display(Button);
}
void button_control_group(button *but,int start,int end,bool (*control)(const button*))
{
    for(int i = start;i < end ;i++)
    {
        control(&but[i]);
    }
}

void goto_home_button_display(bool normal)
{
    resource res;
    if(normal == true) {
        if(goto_layout == &layout_capture)
        {
            goto_home_button.pos.point.y = 15;
            res.id = ROM_R_6_CAPTURE_HOME_BUTTON_OFF_PNG;
            res.size = ROM_R_6_CAPTURE_HOME_BUTTON_OFF_PNG_SIZE;       
        }
        else
        {
            goto_home_button.pos.point.y = 2;
            res.id = ROM_R_5_INTERNAL_CALL_RETURN_PNG;
            res.size = ROM_R_5_INTERNAL_CALL_RETURN_PNG_SIZE;       
        }
    }
    else{
        if(goto_layout == &layout_capture)
        {
            goto_home_button.pos.point.y = 15;
            res.id = ROM_R_6_CAPTURE_HOME_BUTTON_ON_PNG;
            res.size = ROM_R_6_CAPTURE_HOME_BUTTON_ON_PNG_SIZE;       
        }
        else
        {
            goto_home_button.pos.point.y = 2;
            res.id = ROM_R_5_INTERNAL_CALL_RETURN_SELECT_PNG;
            res.size = ROM_R_5_INTERNAL_CALL_RETURN_SELECT_PNG_SIZE;       
        }
    }
    button_refresh(&goto_home_button,res.id,res.size,true);
}
void goto_home_button_up(const void *arg)
{
    app_layout_goto(&layout_home);
}
void goto_home_button_leave_down(const void *arg)
{
    goto_home_button_display(true);
}
void goto_home_button_down(const void *arg)
{
    goto_home_button_display(false);
}
void goto_home_button_init(void)
{
    position pos = {{5,2},{93,89}};
    button_init(&goto_home_button,&pos,goto_home_button_up);
    goto_home_button.down = goto_home_button_down;
    goto_home_button.leave_down = goto_home_button_leave_down;
    //goto_home_button_display(true);
}

bool photo_file_all_clear(int photo_type)
{
    int photo_count;
    photo_count = media_file_total_get(photo_type, false);
    for (int i = 0; i < photo_count; i++) {
        int index = photo_count - 1 - i;
        if (index >= 0) {
            media_file_delete(photo_type, index);
        }
    }
    return true;
}

int layout_get_week(struct ak_date *date){
    int m = date->month + 1;
    int y = date->year;
    int d = date->day + 1;
    if (m == 1 || m == 2) {
        m += 12;
        y--;
    }
    /*1~7*/
    return (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7 ;
}
resource home_DateTime_icon_get(int num,enum time_type type)
{
    resource res;
    switch (num)
    {
    case 0:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_0_PNG : ROM_R_DATE_0_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_0_PNG_SIZE : ROM_R_DATE_0_PNG_SIZE;
        break;
    case 1:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_1_PNG : ROM_R_DATE_1_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_1_PNG_SIZE : ROM_R_DATE_1_PNG_SIZE;
        break;
    case 2:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_2_PNG : ROM_R_DATE_2_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_2_PNG_SIZE : ROM_R_DATE_2_PNG_SIZE;
        break;
    case 3:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_3_PNG : ROM_R_DATE_3_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_3_PNG_SIZE : ROM_R_DATE_3_PNG_SIZE;
        break;
    case 4:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_4_PNG : ROM_R_DATE_4_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_4_PNG_SIZE : ROM_R_DATE_4_PNG_SIZE;
        break;
    case 5:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_5_PNG : ROM_R_DATE_5_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_5_PNG_SIZE : ROM_R_DATE_5_PNG_SIZE;
        break;
    case 6:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_6_PNG : ROM_R_DATE_6_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_6_PNG_SIZE : ROM_R_DATE_6_PNG_SIZE;
        break;
    case 7:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_7_PNG : ROM_R_DATE_7_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_7_PNG_SIZE : ROM_R_DATE_7_PNG_SIZE;
        break;
    case 8:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_8_PNG : ROM_R_DATE_8_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_8_PNG_SIZE : ROM_R_DATE_8_PNG_SIZE;
        break;
    case 9:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_9_PNG : ROM_R_DATE_9_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_9_PNG_SIZE : ROM_R_DATE_9_PNG_SIZE;
        break;
    default:
        res.id = type == CLOCK_TIME ? ROM_R_TIME_COLON_PNG : ROM_R_DATE_SLASH_PNG;
        res.size = type == CLOCK_TIME ? ROM_R_TIME_COLON_PNG_SIZE : ROM_R_DATE_SLASH_PNG_SIZE;
        break;
    }
    return res;
}
void home_date_time_display(struct ak_date *date,enum btn_string_id id,position pos,enum time_type type)
{
    if(type == CLOCK_TIME || type == DATE_TIME)
    {
        icon home_date;
        resource res;
        static bool COLON_REFRESH = false;
        // int num1 = type == CLOCK_TIME ? (date->hour %12 ? date->hour %12/10 : 1) : (date->month +1) %12 ? (date->month + 1) %12/10: 1;//获得第一位
        // int num2 = type == CLOCK_TIME ? (date->hour %12 ? date->hour %12%10: 2) : (date->month +1) %12%10 ? (date->month +1) %12%10 : 2;//第二位
        // int num3 = type == CLOCK_TIME ? (date->minute %60 ? date->minute %60/10: 0) : (date->day + 1) %30/10 ;//第三位
        // int num4 = type == CLOCK_TIME ? (date->minute %60 ? date->minute %60%10: 0) : (date->day + 1) %30%10;//第四位
        int num1,num2,num3,num4;
        if(language_get() == language_russian)
        {
            num1 = type == CLOCK_TIME ? (date->hour/10) : (date->month +1) / 10;//获得第一位
            num2 = type == CLOCK_TIME ? (date->hour %10) : (date->month +1) %10;//第二位
        }
        else
        {
            num1 = type == CLOCK_TIME ? ((date->hour%12) ? date->hour%12/10 : 1) : (date->month +1) / 10;//获得第一位
            num2 = type == CLOCK_TIME ? ((date->hour %12) ? date->hour %12%10 : 2) : (date->month +1) %10;//第二位
        }
        num3 = type == CLOCK_TIME ? (date->minute / 10) : (date->day + 1) /10 ;//第三位
        num4 = type == CLOCK_TIME ? (date->minute % 10) : (date->day + 1) %10;//第四位
        // DEBUG_LOG("BBBBB:%02d  :  %02d\n\r",date->hour,date->minute );
        // DEBUG_LOG("AAAAAAAAAAAAAAAAAAAAAAAAA:%02d.%02d\n\r",num1,num2 );
        res = home_DateTime_icon_get(num1,type);
        icon_init(&home_date,&pos,&res);
        home_date.erase = true;
        icon_display(&home_date);

        pos.point.x += pos.vector.width;
        res = home_DateTime_icon_get(num2,type);
        icon_init(&home_date,&pos,&res);
        home_date.erase = true;
        icon_display(&home_date);

        pos.point.x += pos.vector.width;
        pos.vector.width = type ==  CLOCK_TIME ? 20 : 24;
        res = home_DateTime_icon_get(10,type);
        icon_init(&home_date,&pos,&res);
        if(type ==  CLOCK_TIME && ((COLON_REFRESH = !COLON_REFRESH)))
        {
            gui_erase(&home_date.pos,0x00);
        }
        else
        {
            home_date.erase = true;
            icon_display(&home_date);
        }

        pos.point.x += pos.vector.width;
        pos.vector.width = type == CLOCK_TIME ? 50 : 24;
        res = home_DateTime_icon_get(num3,type);
        icon_init(&home_date,&pos,&res);
        home_date.erase = true;
        icon_display(&home_date);

        pos.point.x += pos.vector.width;
        res = home_DateTime_icon_get(num4,type);
        icon_init(&home_date,&pos,&res);
        home_date.erase = true;
        icon_display(&home_date);
    }
    else
    {
        if(language_get() == language_russian && type == PERIOD_TIME)
            return;

        text home_date;
        char date_str[64];

        text_init(&home_date,&pos,font_size(HOME_DATE_STR));
        // home_date.font_color = BLUE;

        if(type == PERIOD_TIME)
            home_date.align = LEFT_MIDDLE;
        else
            home_date.align = RIGHT_MIDDLE;

        if(type == WEEK_TIME && id > STR_FRI)
            home_date.font_color = 0xffC31A10;
        else
            home_date.font_color = 0xffbfc1c3;

        home_date.erase = true;
        // home_date.bg_color = BLUE;
        if(type == WEEK_TIME || type == PERIOD_TIME)
            language_text_display(&home_date,text_str(id),language_get());
        else
            text_display(&home_date,date_str);
    }
}

void layout_get_LocalDate(struct ak_date *date,int *week)
{
    ak_get_localdate(date);

    if(week != NULL)
        *week = layout_get_week(date);
}
void get_time_str(char *time,int len){
        struct ak_date tmp_date;
        ak_get_localdate(&tmp_date);
        memset(time,0,len);
        sprintf(time, "%d-%02d-%02d %02d:%02d:%02d",tmp_date.year,tmp_date.month+1,tmp_date.day+1,tmp_date.hour,tmp_date.minute,tmp_date.second);   
}

void add_absent_event(int sour_id,int dev_id){
        char time[32];
        char event[48];
        unsigned char channel_str_id = STR_SENSOR1+sour_id;
        unsigned char device_str_id = STR_DEVICE1+dev_id;
        get_time_str(time,sizeof(time));
        // sprintf(ch,"%s",text_str(STR_SENSOR2+sour_id));
        sprintf(event,"%s",time);
        // absent_event_enqueue(&absent_event,event);
        absent_event_array_upadte(&absent_event_group,event,channel_str_id,device_str_id);
        // DEBUG_LOG("add event ============>%s\n",event);
        // absent_event_array_traversal(&absent_event_group);
}
void disable_listens_button(button**pbtn_group,int *index){
    button_listens_copy(pbtn_group,index);
    button_listens_destroy();
}
void enable_listens_button(button**pbtn_group,int *index){
    button_listens_destroy();
    button_listens_recovery(pbtn_group,*index);
    for (int i = 0 ; i < *index ; i++) {
        button_enable(pbtn_group[i]);
    }
}

void date_set(struct ak_date *date)
{
    char date_param[32] = {0};
    sprintf(date_param, "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"",
            date->year,
            date->month + 1,
            date->day + 1,
            date->hour,
            date->minute,
            date->second);
    system(date_param);
    system("hwclock -w");
    DEBUG_LOG("%s         sizeof: %d\n\r",date_param,sizeof(date_param));
    net_common_data_send(DEVICE_ALL,NET_CMD_LOCAL_TIME_SYN_ACK,(unsigned char*)date_param,sizeof(date_param));
}

void get_network_time(void){
    struct tm local_time;
    bool ret = tuya_net_time_sync(&local_time);
    if(ret == true){
        char date_param[32] = {0};
        sprintf(date_param, "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"",
                local_time.tm_year,
                local_time.tm_mon,
                local_time.tm_mday,
                local_time.tm_hour,
                local_time.tm_min,
                local_time.tm_sec);
        system(date_param);
        system("hwclock -w");
        // DEBUG_LOG("%s         sizeof: %d\n\r",date_param,sizeof(date_param));
        ak_get_localdate(&user_data_get()->user_default_time);
        net_common_data_send(DEVICE_ALL,NET_CMD_LOCAL_TIME_SYN_ACK,(unsigned char*)date_param,sizeof(date_param));
    }
}
void Audio_play_stop(void){
    extern void amp_enable(bool);
    amp_enable(false);
    if(is_audio_play_ing() && audio_playing_index() != KEY_1){
        DEBUG_LOG("--------------------------------------------------->%s\n\r",__func__);
        audio_play_stop();
    }
    amp_enable(true);
}                   
void count_display( int conut,bool is_erase)
{
    position pos = {{944,30},{50,50}};
    gui_erase(&pos,0x00);
    if(is_erase) return;

    icon bg;
    text counter;
    resource res = resource_get(ROM_R_4_DOOR_CAM_TIME_BG_PNG);
    char date_str[5];
    int Font_size;
    sprintf(date_str, "%d",conut);  
    Font_size =  20;
    icon_init(&bg,&pos,&res);
    icon_display(&bg);
    text_init(&counter,&pos,Font_size);
    text_display(&counter,date_str);
}
void wallpaper_dispaly(wallpaper_type res_type)
{
    resource res = resource_get(ROM_R_0_IMG_MAIN_BG_JPG);
    switch (res_type)
    {
    case SET:
        res.id = ROM_R_0_IMG_SET_JPG;
        res.size = ROM_R_0_IMG_SET_JPG_SIZE;
        break;
    case CALL_BG:
        res.id = ROM_R_0_IMG_CALL_BG_JPG;
        res.size = ROM_R_0_IMG_CALL_BG_JPG_SIZE;
        break;
    case ALARM_RED:
        // res.id = ROM_R_0_IMG_ALARM_RED_JPG;
        // res.size = ROM_R_0_IMG_ALARM_RED_JPG_SIZE;
        break;
    case ALARM_BLUE:
        // res.id = ROM_R_0_IMG_ALARM_BLUE_JPG;
        // res.size = ROM_R_0_IMG_ALARM_BLUE_JPG_SIZE;
        break;
    case KEY_BOARD:
        res.id = ROM_R_0_IMG_KEY_BOARD_JPG;
        res.size = ROM_R_0_IMG_KEY_BOARD_JPG_SIZE;
        break;    
    case CLOCK_1:
        res.id = ROM_R_0_IMG_CLOCK1_JPG;
        res.size = ROM_R_0_IMG_CLOCK1_JPG_SIZE;
        break;   
    case CLOCK_2:
        res.id = ROM_R_0_IMG_CLOCK2_JPG;
        res.size = ROM_R_0_IMG_CLOCK2_JPG_SIZE;
        break;   
    case CLOCK_3:
        res.id = ROM_R_0_IMG_CLOCK3_JPG;
        res.size = ROM_R_0_IMG_CLOCK3_JPG_SIZE;
        break;   
    case CLOCK_4:
        res.id = ROM_R_0_IMG_CLOCK4_JPG;
        res.size = ROM_R_0_IMG_CLOCK4_JPG_SIZE;
        break;   
    case CLOCK_5:
        res.id = ROM_R_0_IMG_CLOCK5_JPG;
        res.size = ROM_R_0_IMG_CLOCK5_JPG_SIZE;
        break;    
    // case TUYA_RUN:
    //     res.id = ROM_R_4_DOOR_CAM_APP_LINE_JPG;
    //     res.size = ROM_R_4_DOOR_CAM_APP_LINE_JPG_SIZE;
    //     break;
    default:
        break;
    }

    position pos = {{0,0},{1024,600}};
    bg_resource_load(&res,&pos,NULL,CENTER_MIDDLE);
}

void clock_display(int x,int y){
    position pos = {{83,70},{336,336}};
    pos.point.x = x;
    pos.point.y = y;
    analog_clock calendar_clock = {
            .bg_res = resource_get(ROM_R_1_HOME_CLOCK_PNG),
            .hour_res = resource_get(ROM_R_1_HOME_HOUR_PNG),
            .min_res = resource_get(ROM_R_1_HOME_MINUTES_PNG),
            .hour_center_y = 60,
            .min_center_y = 78,
            .pos = pos
    };
    analog_clock_init(&calendar_clock);
    analog_clock_update();
}

void free_pointer(char **p)
{
    if(*p !=NULL)
    {
        free(*p);
        *p = NULL;        
        if(*p == NULL)
        DEBUG_LOG("free-------------->>>succee\n\r");        
    }
}
/*从字符串的中间截取n个字符*/
char * mid(char *dst,char *src, int n,int m) /*n为长度，m为位置*/
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len-m;    /*从第m个到最后*/
    if(m<0) m=0;    /*从第一个开始*/
    if(m>len) return NULL;
    p += m;
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; /*有必要吗？很有必要*/
    return dst;
}
void disable_prev_lay_button(button** pbtn_group,int* index){
    button_listens_copy(pbtn_group,index);
    button_listens_destroy();
}
void enable_prev_lay_button(button** pbtn_group,int* index){
    button_listens_destroy();
    button_listens_recovery(pbtn_group,*index);
    for (int i = 0 ; i < *index ; i++) {
        button_enable(pbtn_group[i]);
        button_display(pbtn_group[i]);
    }
}
bool button_tone_mute(const button*BUT){
    int *tone =  (int*)&BUT->button_tone;
    *tone = -1;
        return true;
}
bool button_tone_open(const button*BUT){
    int *tone =  (int*)&BUT->button_tone;
    *tone = KEY_1;
        return true;
}


bool is_point_in_scope(const point* point,const position* rect){

    if((point->x < rect->point.x)||(point->y < rect->point.y)){
        return false;
    }
    if((point->x >(rect->point.x + rect->vector.width)) ||(point->y > (rect->point.y + rect->vector.height))){
        return false;
    }
    return true;
}
int echo(char *value,char *path)
{
    FILE *fb = fopen(path,"wb");
    fwrite(value,1,strlen(value),fb);
    fclose(fb);
    return 0;
}
int output_file_use(char *cata)
{ 
    FILE *fp; 
    int use;
    char buffer[80]; 
    char com[64];
    sprintf(com, "df  | grep '%s'| awk '{print $(NF-1)}' | awk -F'%%' '{print $1}'",cata);
    fp=popen(com, "r"); 
    fgets(buffer,sizeof(buffer),fp); 
    use = atoi(buffer);    
    if(use > 98) DEBUG_LOG("%s Use Exceed 98\n\r",cata);
    pclose(fp);
    return use;
}
void app_layout_goto(layout* layout)
{    
    // DEBUG_LOG("%s ---------------------> %d\n\r",__func__,__LINE__);
    // if(goto_layout != layout)
    // bool tuya_key_uuid_check(void);
    // if(tuya_key_uuid_check() == false){
    //     return;
    // }
    prev_layout = goto_layout;

    goto_layout = layout;
    layout_switch = true;
    os_layout_goto(layout);
}
void goto_prev_layout(void)
{    
    // DEBUG_LOG("%s ---------------------> %d\n\r",__func__,__LINE__);
    app_layout_goto(prev_layout);
}

void confirm_interface(enum btn_string_id str_id1,enum btn_string_id str_id2,bool need_erasure,unsigned int color,int font_size)
{
    position pos = {{0,0},{1024,600}};
    position pos_on = {{180,290},{664,1}};
    position pos_below = {{180,449},{664,1}};
    if(need_erasure)
    {
        gui_erase(&pos,0x00);
        return;
    }
    draw_rect(&pos,color);
    draw_rect(&pos_on,GRAY);
    draw_rect(&pos_below,GRAY);
    text hint_text;
    position text_pos = {{0,175},{1024,48}};
    text_init(&hint_text,&text_pos,font_size);
    language_text_display(&hint_text,text_str(str_id1),language_get());         
    if(str_id2 != STR_TOTAL){
        text_pos.point.y = 215;
        text_init(&hint_text,&text_pos,font_size);
        language_text_display(&hint_text,text_str(str_id2),language_get());         
    }



}

bool return_disturb_status(void)
{
    return user_data_get()->do_not_disturb;
}

void headline_display(bool head_display)
{
    position wire = {{0,105},{1024,1}};
    draw_rect(&wire,0xff818080);
    if(head_display){
        position head = {{0,0},{1024,105}};
        draw_rect(&head,BG_COLOR);        
    }

}
void language_text_display(text *text,const char* str,enum language language){
    text->def_language = language;
    text_display(text,str);
}
// void language_text1_display(text *text,enum btn_string_id id){
//     text->def_language = language_get();
//     text_display(text,text_str(id));
// }