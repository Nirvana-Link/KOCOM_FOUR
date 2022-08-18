#include"layout_absent_button.h"
#include"language.h"
#include"stdlib.h"
#include"os_sys_api.h"
#include"layout_common.h"
#include"l_absent_queue.h"
#include"l_absent_ev.h"
#include"l_audio_play.h"
#include"tuya_sdk.h"
#include"debug.h"

#define cur_page_frisr_ev  ((event_page - 1)*EVENT_BUT_TOTAL)
#ifdef EVENT_QUEUE
static queue_ptr cur_page_frist_ev_ptr = NULL;
#endif
static unsigned int event_num;
static unsigned int event_page;
static unsigned int tatol_page;
static bool View_events = false;

static int absent_start_count = 0;



static void clear_page(bool full_clear){
    position pos = {{58,95},{928,444}};
    if(full_clear)
        pos.vector.height = 500;
    gui_erase(&pos,0x00);
}
#ifdef EVENT_QUEUE
static void cur_ev_frist_ptr_set(bool is_next,queue_ptr *cur_ptr){
    int ev_tra = EVENT_BUT_TOTAL;

    *cur_ptr = cur_page_frist_ev_ptr;    
    if((cur_page_frist_ev_ptr == absent_event.tail) && (is_next == false))
    return;


    while(ev_tra > 0){//遍历至当前页的最后一个事件
        if(*cur_ptr == absent_event.head)  return;
        // DEBUG_LOG("event :%s  \n\r",(*cur_ptr)->event);
        *cur_ptr = (is_next == true) ? (*cur_ptr)->prev : (*cur_ptr)->next;
        ev_tra--;
    }
    cur_page_frist_ev_ptr = *cur_ptr;
}

static void event_item_diplay(queue event,bool is_next){
    text ev;
    queue_ptr cur_ptr = NULL;
    int ev_id= 0;
    position pos = {{250,94},{700,72}};
    position wire = {{58,166},{928,1}};
    clear_page(false);
    cur_ev_frist_ptr_set(is_next,&cur_ptr);
    while((cur_ptr != absent_event.head) && (ev_id < EVENT_BUT_TOTAL)){//继续遍历至需显示的事件
        // abs_ev_but_display(cur_ptr,&ABS_EVENT_BUT[ev_id]);
        // button_enable(&ABS_EVENT_BUT[ev_id]);
        
        text_init(&ev,&pos,font_size(ABSENT_INFO));
        if(strncmp(cur_ptr->event,"CCTV",4) == 0){
            ev.font_color = RED;
            pos.point.x = 250;
        }else{
            ev.font_color = BLUE;
            pos.point.x = 250;
        }
        ev.align = LEFT_MIDDLE;
        pos.point.y += 72;
        text_display(&ev,cur_ptr->event);
        cur_ptr = cur_ptr->prev;
        
        draw_rect(&wire,GRAY);
        wire.point.y += 72;
        ev_id++;
    }
}
#endif

void event_icon_display(int ev_id)
{
    icon event_icon;
    resource res = resource_get(ROM_R_7_ABSENT_ALARM_PNG);
    position pos = {{150,110},{36,36}};
    pos.point.y += (ev_id * 72);
    icon_init(&event_icon,&pos,&res);
    icon_display(&event_icon); 
}

#ifdef EVENT_ARRAY
static void event_item_diplay(absent_ev_array event,bool is_next){
    text data_time;
    text device;
    text channel;
    int ev_id= 0;
    position pos_t = {{235,94},{190,72}};
    position pos_d = {{420,94},{165,72}};
    position pos_c = {{590,94},{100,72}};
    position wire = {{58,166},{928,1}};
    clear_page(false);
    while(ev_id < EVENT_BUT_TOTAL && (cur_page_frisr_ev + ev_id) < event.size){//继续遍历至需显示的事件
        // abs_ev_but_display(cur_ptr,&ABS_EVENT_BUT[ev_id]);
        // button_enable(&ABS_EVENT_BUT[ev_id]);
        event_icon_display(ev_id);

        text_init(&data_time,&pos_t,font_size(ABSENT_INFO) - 4);

        text_init(&device,&pos_d,font_size(ABSENT_INFO) - 4);
        // data_time.bg_color = BLUE;
        text_init(&channel,&pos_c,font_size(ABSENT_INFO) - 4);
        // channel.bg_color = RED;

        channel.align = LEFT_MIDDLE;
        data_time.align = LEFT_MIDDLE;
        pos_c.point.y = pos_d.point.y = pos_t.point.y += 72;
        language_text_display(&channel,text_str(event.channel[cur_page_frisr_ev + ev_id]),language_get());
        language_text_display(&device,text_str(event.device[cur_page_frisr_ev + ev_id]),language_get());
        text_display(&data_time,(const char*)event.absent_ev_group[cur_page_frisr_ev + ev_id]);
        draw_rect(&wire,GRAY);
        wire.point.y += 72;
        ev_id++;
    }
}
#endif
static void get_event_page(absent_ev_array event){
    DEBUG_LOG("%s : %d\n\r",__func__,event.size);
    event_num = event.size;
    event_page = 1;
    tatol_page = event_num/EVENT_BUT_TOTAL+(((event_num < EVENT_BUT_TOTAL) || (event_num%EVENT_BUT_TOTAL) > 0 ) ? 1: 0);
}
static void event_page_display(void){
    text page;
    position pos = {{468,542},{140,50}};
    char page_str[12];
    sprintf(page_str,"%02d / %02d",event_page,tatol_page);
    text_init(&page,&pos,font_size(ABSENT_INFO) - 4);
    gui_erase(&pos,0x00);
    text_display(&page,page_str);
}

static void event_button_display(void){
    View_events = false;
    button_refresh(&ABS_COMMON_BUT[EVENT_BUTTON],ROM_R_7_ABSENT_EVENT_PNG,ROM_R_7_ABSENT_EVENT_PNG_SIZE,true);
}


static button *backup_button_group[BUTTON_MAX];//被销毁之前的按键集合
static int backup_button_total = 0;//被销毁的总数
/* 确认删除函数 */
static void clear_ack_oper(void)
{
    confirm_interface(STR_DELETE_ALL_EVENT,STR_TOTAL,false,VERY_TRANSPARENT,font_size(ABSENT_INFO) + 4);
    disable_prev_lay_button(backup_button_group,&backup_button_total); //销毁当前所有按键     
    button_start_using(&ABS_COMMON_BUT[CLEAR_ACK]);//使能确认键
    button_start_using(&ABS_COMMON_BUT[CLEAR_CANCEL]);//使能取消键
}

static void  abs_com_button_up(const void *arg)
{
    int but_id = SequenceSearch(ABS_COMMON_BUT,(Par_Button),ABSENT_COM_BUT_TOTAL);

    switch (but_id)
    {
    // case DEL_BUTTON:

    //     break;
    case DEL_ALL_BUTTON:
        if(event_num != 0)
            clear_ack_oper();
        break;
    case PREV_BUTTON:
        // if(event_page == 1) return; 
        event_page = event_page == 1 ? tatol_page : event_page -1;
        // button_control_group(ABS_EVENT_BUT,EVENT1_BUTTON,EVENT_BUT_TOTAL,button_start_disable);
        event_item_diplay(absent_event_group,false);
        event_page_display();
        break;
    case NEXT_BUTTON:
        // if(tatol_page == event_page) return;
        event_page = tatol_page == event_page ? 1 : event_page + 1;
        // button_control_group(ABS_EVENT_BUT,EVENT1_BUTTON,EVENT_BUT_TOTAL,button_start_disable);
        event_item_diplay(absent_event_group,true);
        event_page_display();
        break;
    case EVENT_BUTTON:
        if(View_events ==  false){
            View_events = true;
            event_page = 1;
            button_refresh((Par_Button),ROM_R_7_ABSENT_EVENT_ON_PNG,ROM_R_7_ABSENT_EVENT_ON_PNG_SIZE,true); 
            event_item_diplay(absent_event_group,false);
            get_event_page(absent_event_group);
            event_page_display();
            button_control_group(ABS_SENSOR_BUT,POWER_BUTTON,ACTIVE_NOW_BUTTON,button_disable);  
            button_control_group(ABS_COMMON_BUT,DEL_ALL_BUTTON,EVENT_BUTTON,button_start_using);  
        }
        else{
            View_events = false;
            clear_page(true);
            button_refresh((Par_Button),ROM_R_7_ABSENT_EVENT_PNG,ROM_R_7_ABSENT_EVENT_PNG_SIZE,true);
            button_control_group(ABS_SENSOR_BUT,POWER_BUTTON,ACTIVE_NOW_BUTTON,button_start_using);  
            button_control_group(ABS_COMMON_BUT,DEL_ALL_BUTTON,EVENT_BUTTON,button_start_disable);
        }
        break;  
    case CLEAR_ACK:
        confirm_interface(STR_TOTAL,STR_TOTAL,true,VERY_TRANSPARENT,font_size(ABSENT_INFO) + 4);
        enable_prev_lay_button(backup_button_group,&backup_button_total);
        absent_event_array_init(&absent_event_group);
        event_item_diplay(absent_event_group,false);
        tatol_page = event_page = 1;
        event_page_display();
        system("rm -f "ABSENT_EV_FILE_PATH); 
        break; 
    case CLEAR_CANCEL:
        confirm_interface(STR_TOTAL,STR_TOTAL,true,VERY_TRANSPARENT,font_size(ABSENT_INFO) + 4);
        event_page_display();
        enable_prev_lay_button(backup_button_group,&backup_button_total);    
        button_control_group(ABS_COMMON_BUT,CLEAR_ACK,ABSENT_COM_BUT_TOTAL,button_disable);  
        event_item_diplay(absent_event_group,false);
        break; 
    default:
        break;
    }
}
static void  abs_com_button_leave_down(const void *arg)
{
    int but_id = SequenceSearch(ABS_COMMON_BUT,(Par_Button),ABSENT_COM_BUT_TOTAL);
    switch (but_id)
    {
    // case DEL_BUTTON:
    //     button_refresh((Par_Button),ROM_R_6_CAPTURE_CLEAR_PNG,ROM_R_6_CAPTURE_CLEAR_PNG_SIZE,true);
    //     break;
     case DEL_ALL_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_CLEAR_ALL_PNG,ROM_R_6_CAPTURE_CLEAR_ALL_PNG_SIZE,true);
        break;
    case PREV_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_LEFT_PNG,ROM_R_6_CAPTURE_LEFT_PNG_SIZE,true);
        break;
    case NEXT_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_RIGHT_PNG,ROM_R_6_CAPTURE_RIGHT_PNG_SIZE,true);
        break;
    case EVENT_BUTTON:
        button_refresh((Par_Button),ROM_R_7_ABSENT_EVENT_PNG,ROM_R_7_ABSENT_EVENT_PNG_SIZE,true);
        break;   
    case CLEAR_ACK:
    case CLEAR_CANCEL:
        button_refresh((Par_Button),ROM_R_2_SET_ACK_BUTTON_PNG,ROM_R_2_SET_ACK_BUTTON_PNG_SIZE,true);
        break; 
    default:
        break;
    }
}
static void  abs_com_button_down(const void *arg)
{
    int but_id = SequenceSearch(ABS_COMMON_BUT,(Par_Button),ABSENT_COM_BUT_TOTAL);
    switch (but_id)
    {
    // case DEL_BUTTON:
    //     button_refresh((Par_Button),ROM_R_6_CAPTURE_CLEAR_SELECT_PNG,ROM_R_6_CAPTURE_CLEAR_SELECT_PNG_SIZE,true);
    //     break;
     case DEL_ALL_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_CLEAR_ALL_SELECT_PNG,ROM_R_6_CAPTURE_CLEAR_ALL_SELECT_PNG_SIZE,true);
        break;
    case PREV_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_LEFT_SELECT_PNG,ROM_R_6_CAPTURE_LEFT_SELECT_PNG_SIZE,true);
        break;
    case NEXT_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_RIGHT_SELECT_PNG,ROM_R_6_CAPTURE_RIGHT_SELECT_PNG_SIZE,true);
        break;
    case EVENT_BUTTON:
        button_refresh((Par_Button),ROM_R_7_ABSENT_EVENT_ON_PNG,ROM_R_7_ABSENT_EVENT_ON_PNG_SIZE,true);
        break;   
    case CLEAR_ACK:
    case CLEAR_CANCEL:
        button_refresh((Par_Button),ROM_R_2_SET_ACK_BUTTON_ON_PNG,ROM_R_2_SET_ACK_BUTTON_ON_PNG_SIZE,true);
        break; 
    default:
        break;
    }
}
static void  abs_com_button_init(enum absent_common_but but,position pos,resource res,bool need_icon)
{
        button_init(&ABS_COMMON_BUT[but],&pos, abs_com_button_up);
        ABS_COMMON_BUT[but].down = abs_com_button_down;
        ABS_COMMON_BUT[but].leave_down = abs_com_button_leave_down;
        if(need_icon)
        button_icon_init(&ABS_COMMON_BUT[but],&res);
}
static void  abs_com_button_list(void)
{
    // DEBUG_LOG("------------------>%s\n\r",__func__);
    {       
        position pos = {{926,11},{64,64}};
        resource res = resource_get(ROM_R_7_ABSENT_EVENT_PNG);
        abs_com_button_init(EVENT_BUTTON,pos,res,true);
    }

    {       
        position pos = {{831,11},{64,64}};
        // position pos1 = {{751,11},{64,64}};
        // resource res1 = resource_get(ROM_R_6_CAPTURE_CLEAR_PNG);
        resource res2 = resource_get(ROM_R_6_CAPTURE_CLEAR_ALL_PNG);

        // abs_com_button_init(DEL_BUTTON,pos1,res1);
        abs_com_button_init(DEL_ALL_BUTTON,pos,res2,true);
    }

    {
        position pos = {{374,538},{70,60}};
        resource res1 = resource_get(ROM_R_6_CAPTURE_LEFT_PNG);
        resource res2 = resource_get(ROM_R_6_CAPTURE_RIGHT_PNG);

        abs_com_button_init(PREV_BUTTON,pos,res1,true);
        pos.point.x = 634;
        abs_com_button_init(NEXT_BUTTON,pos,res2,true);
    }

    {
        position pos = {{284,341},{210,60}};  
        resource res = resource_get(ROM_R_2_SET_ACK_BUTTON_PNG);
        abs_com_button_init(CLEAR_ACK,pos,res,true);
        pos.point.x = 552;
        abs_com_button_init(CLEAR_CANCEL,pos,res,true);
        button_text_init(&ABS_COMMON_BUT[CLEAR_ACK],btn_str(STR_YES),font_size(ABSENT_INFO));
        button_text_init(&ABS_COMMON_BUT[CLEAR_CANCEL],btn_str(STR_NO),font_size(ABSENT_INFO));
    }
}

static void active_now_btn_display(void) {

    static char *str[language_total] = {"Active Now","즉시 실행","Activo ahora","الآن مفعل ","Đang hoạt động","Активен"};
    if (user_data_get()->user_default_language == VIETNAMESE) {
        button_text_init(&ABS_SENSOR_BUT[ACTIVE_NOW_BUTTON], str, font_size(ABSENT_INFO) - 6);
    } else {
        button_text_init(&ABS_SENSOR_BUT[ACTIVE_NOW_BUTTON], str, font_size(ABSENT_INFO) - 4);
    }
    button_display(&ABS_SENSOR_BUT[ACTIVE_NOW_BUTTON]);
}
static void delay_time_display(unsigned int delay) {
    position pos = {{0,    0},
                    {1024, 600}};
    //gui_erase(&pos,0xffff0000);
    text notice;
    const char *str[language_total] = {"seconds, the absent mode starts to run."," 초 후 외출모드 실행"," segundos, comienza a funcionar el modo ausente.","بعد","giây, chế độ vắng mặt bắt đầu chạy.","секунд запустится режим отсутствия."};
    const char *str0[language_total] = {"After","","Después de ","ثانية ، سيبدأ تشغيل وضع الغائب .","Sau","Через"};
    char tmp_str[128] = {0};
    memset(tmp_str, 0, sizeof(tmp_str));
    if (language_get() == language_arabic) {
        gui_erase(&pos, 0xffff0000);
        position pos0 = {{73,  249},
                         {600, 80}};

        text_init(&notice, &pos0, font_size(ABSENT_INFO) + 6);
        notice.align = RIGHT_MIDDLE;
        language_text_display(&notice, str0[language_get()],language_get());

        position pos1 = {{720, 249},
                         {300, 80}};
        text_init(&notice, &pos1, font_size(ABSENT_INFO) + 6);
        notice.align = LEFT_MIDDLE;
        language_text_display(&notice, str[language_get()],language_get());

        position pos3 = {{677, 249},
                         {40,  80}};
        sprintf(tmp_str, "%d", delay);
        text_init(&notice, &pos3, font_size(ABSENT_INFO) + 6);
        language_text_display(&notice, tmp_str,language_english);

    } else {
        sprintf(tmp_str, "%s  %d  %s",
                str0[language_get()],
                delay,
                str[language_get()]);
        text_init(&notice, &pos, font_size(ABSENT_INFO) + 6);
        notice.erase = true;
        notice.bg_color = 0xffe53f3f;
        language_text_display(&notice, tmp_str,language_get());
    }
    active_now_btn_display();
}

static void sensor_power_resource(bool open){
    resource res1_off = resource_get(open ? ROM_R_7_ABSENT_ABSENT_ON_PNG : ROM_R_7_ABSENT_ABSENT_OFF_PNG);
    ABS_SENSOR_BUT[POWER_BUTTON].font_color = (open == true)?BLACK:WHITE;
    ABS_SENSOR_BUT[POWER_BUTTON].string = btn_str(open ? STR_ABSNET_STOP : STR_ABSNET_START);
    button_icon_init(&ABS_SENSOR_BUT[POWER_BUTTON],&res1_off);
}
static void sensor_button_resource(enum absent_sensor_but but){
    resource res_on = resource_get(ROM_R_7_ABSENT_SENSOR_ON_PNG);
    resource res_off = resource_get(ROM_R_7_ABSENT_SENSOR_PNG);
    bool open_flag = but == SENSOR1_BUTTON ?  user_data_get()->security.Sensor1_open : user_data_get()->security.Sensor2_open;
    
    button_icon_init(&ABS_SENSOR_BUT[but],open_flag ? &res_on : &res_off);
    ABS_SENSOR_BUT[but].font_color = open_flag ? BLUE : GRAY;
}

static void sensor_status_change(bool runing){
        user_data_get()->security.running = runing;

        ABS_SENSOR_BUT[POWER_BUTTON].button_tone = !user_data_get()->security.running && !user_data_get()->security.Sensor1_open && !user_data_get()->security.Sensor2_open ? KEY_2 : KEY_1;
        tuya_dp_232_response_absent_mode(user_data_get()->security.running);

        int arg1 = user_data_get()->security.running | user_data_get()->security.Sensor1_open << 1 | user_data_get()->security.Sensor2_open << 2;
        net_common_send(DEVICE_ALL,NET_CMD_ABSENT_POWER,arg1,0);
}
static void sensor_button_up(const void *arg)
{
    if(Par_Button == &ABS_SENSOR_BUT[SENSOR1_BUTTON])
    {
        if(user_data_get()->security.Sensor1_open)
        {
            user_data_get()->security.Sensor1_open = false;
        }
        else
        {
            user_data_get()->security.Sensor1_open = true;
        }
        sensor_button_resource(SENSOR1_BUTTON);
    }
    else if(Par_Button == &ABS_SENSOR_BUT[SENSOR2_BUTTON])
    {
        if(user_data_get()->security.Sensor2_open)
        {
            user_data_get()->security.Sensor2_open = false;
        }
        else
        {
            user_data_get()->security.Sensor2_open = true;
        }
        sensor_button_resource(SENSOR2_BUTTON);
    }
    else if(Par_Button == &ABS_SENSOR_BUT[POWER_BUTTON] && ABS_SENSOR_BUT[POWER_BUTTON].button_tone != KEY_2)
    {

        if(user_data_get()->security.running == false)
        {
            button_listens_destroy();
            absent_start_count = user_data_get()->user_default_mode_conf.user_default_absent_delay +1;
            button_enable(&ABS_SENSOR_BUT[ACTIVE_NOW_BUTTON]);
            Par_Button->string = btn_str(STR_ABSNET_STOP);
        }
        else
        {
            user_data_get()->security.Sensor1_open = user_data_get()->security.Sensor2_open = false;
            sensor_status_change(false);
            // sensor_button_resource(&ABS_SENSOR_BUT[SENSOR1_BUTTON]);
            // sensor_button_resource(&ABS_SENSOR_BUT[SENSOR2_BUTTON]);
            // button_display(&ABS_SENSOR_BUT[SENSOR1_BUTTON]);
            // button_display(&ABS_SENSOR_BUT[SENSOR2_BUTTON]);
            Par_Button->string = btn_str(STR_ABSNET_START);
            button_display(Par_Button);
        }
        return;
    }
    else if(Par_Button == &ABS_SENSOR_BUT[ACTIVE_NOW_BUTTON])
    {
        sensor_status_change(true);
        app_layout_goto(&layout_home);
        return;
    }
    sensor_status_change(user_data_get()->security.running);
}
static void sensor_button_init(button *Button,position pos,resource res,enum btn_string_id id,align font_ailgn)
{
    
        button_init(Button,&pos,sensor_button_up);
        button_icon_init(Button,&res);
        button_text_init(Button,btn_str(id),font_size(ABSENT_BUTTON) - 2);
        Button->font_align = font_ailgn;
        Button->erase = true;
}
static void sensor_button_list(void)
{
    position pos = {{325,180},{148,250}};
    position pos1 = {{442,474},{140,55}};
    resource res_on = resource_get(ROM_R_7_ABSENT_SENSOR_ON_PNG);
    resource res_off = resource_get(ROM_R_7_ABSENT_SENSOR_PNG);

    resource res1_off = resource_get(ROM_R_7_ABSENT_ABSENT_OFF_PNG);
    resource res1_on = resource_get(ROM_R_7_ABSENT_ABSENT_ON_PNG);
    
    sensor_button_init(&ABS_SENSOR_BUT[SENSOR1_BUTTON],pos,((user_data_get()->security.Sensor1_open == true)?res_on:res_off),STR_SENSOR1,CENTER_BOTTOM);
    ABS_SENSOR_BUT[SENSOR1_BUTTON].font_color = (user_data_get()->security.Sensor1_open == true)?BLUE:WHITE;

    pos.point.x = 552;
    sensor_button_init(&ABS_SENSOR_BUT[SENSOR2_BUTTON],pos,((user_data_get()->security.Sensor2_open == true)?res_on:res_off),STR_SENSOR2,CENTER_BOTTOM);
    ABS_SENSOR_BUT[SENSOR2_BUTTON].font_color = (user_data_get()->security.Sensor2_open == true)?BLUE:WHITE;

    sensor_button_init(&ABS_SENSOR_BUT[POWER_BUTTON],pos1,user_data_get()->security.running ? res1_on : res1_off,STR_ABSNET_START,CENTER_MIDDLE);
    ABS_SENSOR_BUT[POWER_BUTTON].font_color = (user_data_get()->security.running == true)?BLACK:WHITE;
    ABS_SENSOR_BUT[POWER_BUTTON].button_tone = (user_data_get()->security.Sensor1_open || user_data_get()->security.Sensor2_open)?KEY_1:KEY_2;
    ABS_SENSOR_BUT[POWER_BUTTON].font_size = font_size(ABSENT_BUTTON);

    sensor_button_init(&ABS_SENSOR_BUT[ACTIVE_NOW_BUTTON],pos1,res1_on,STR_ACTION_NOW,CENTER_MIDDLE);
    ABS_SENSOR_BUT[ACTIVE_NOW_BUTTON].font_color = BLACK;
    ABS_SENSOR_BUT[ACTIVE_NOW_BUTTON].erase = false;
}


static void absent_button_display(void)
{
    // headline_display();
    // headline_display(false);
    goto_home_button_display(true);
    sensor_power_resource(user_data_get()->security.running);
    sensor_button_resource(SENSOR1_BUTTON);
    sensor_button_resource(SENSOR2_BUTTON);
    button_control_group(ABS_SENSOR_BUT,POWER_BUTTON,ACTIVE_NOW_BUTTON,button_display);   
    event_button_display(); 
}
static void absent_button_enable(void)
{
    button_enable(&goto_home_button);
    button_control_group(ABS_SENSOR_BUT,POWER_BUTTON,ACTIVE_NOW_BUTTON,button_enable);  
    button_enable(&ABS_COMMON_BUT[EVENT_BUTTON]); 
}
static void layout_absent_init(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    goto_home_button_init();
    abs_com_button_list();
    // abs_ev_button_list();
    sensor_button_list();
}

static void layout_absent_enter(void)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    absent_start_count = 0;
    set_gui_background_color(MAIN_BG_COLOUR);
    absent_button_display();
    absent_button_enable();
}

static void layout_absent_quit(void)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
}

static void layout_absent_timer(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    static bool curr_absent_status = false;
    static bool curr_sensor1_status = false;
    static bool curr_sensor2_status = false;
    struct ak_timeval tv;
    ak_get_ostime(&tv);
    static unsigned long prev_sec = 0;
    if(absent_start_count > 0&& tv.sec != prev_sec )
    {
        prev_sec = tv.sec;
        delay_time_display(--absent_start_count);
        // DEBUG_LOG("absent_start_count =================>%d\n\r",absent_start_count);
        if(absent_start_count == 0)
        {
            sensor_status_change(true);
            app_layout_goto(&layout_home);
            return;
        }
    }
    else if(absent_start_count == 0)
    {
        if(curr_absent_status != user_data_get()->security.running || 
            curr_sensor1_status != user_data_get()->security.Sensor1_open ||
            curr_sensor2_status != user_data_get()->security.Sensor2_open){

            curr_absent_status = user_data_get()->security.running;
            curr_sensor1_status = user_data_get()->security.Sensor1_open;
            curr_sensor2_status = user_data_get()->security.Sensor2_open;
            DEBUG_LOG("curr_sensor1_status :%d   ,curr_sensor2_status:%d\n\r",curr_sensor1_status,curr_sensor2_status);
            sensor_power_resource(user_data_get()->security.running);
            sensor_button_resource(SENSOR1_BUTTON);
            sensor_button_resource(SENSOR2_BUTTON);
            button_control_group(ABS_SENSOR_BUT,POWER_BUTTON,ACTIVE_NOW_BUTTON,button_display);   
        }        
    }

}

layout layout_absent = 
{
    .init = layout_absent_init,
    .enter = layout_absent_enter,
    .quit = layout_absent_quit,
   .timer = layout_absent_timer
};