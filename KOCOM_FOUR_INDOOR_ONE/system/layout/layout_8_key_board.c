#include"string.h"
#include"malloc.h"
#include"ui_api.h"
#include"os_sys_api.h"
#include"layout_common.h"
#include"layout_keyborad_button.h"
#include"l_user_config.h"
#include"l_cctv_conf.h"
#include"l_wifi.h"
#include"l_audio_play.h"
#include"l_network.h"
#include "audio_play_api.h"
#include"network_function_api.h"
#include"debug.h"

#define BUFFER_SIZE         16
static char TEXT_INPUT_BUFFER[BUFFER_SIZE];
static int TextInputIndex = 0;
static bool is_input_hide = true;
static bool is_password_error = false;
static bool is_switch_symb = false;   //切换符号
static bool is_switch_big = false;   //切换大小写     
static button KEY_BOARD_BUT[45];
static keyboard_info KEY_BOARD_INFO[45] = {
    {0, {{15, 210},{75, 75}}, {"1", "1","~"}, 1},
    {1, {{97, 210},{ 75, 75}}, {"2", "2","!"}, 1},
    {2, {{179, 210},{ 75, 75}}, {"3", "3","@"}, 1},
    {3, {{261, 210},{ 75, 75}}, {"4", "4","#"}, 1},
    {4, {{343, 210},{ 75, 75}}, {"5", "5","$"}, 1},
    {5, {{425, 210},{ 75, 75}}, {"6", "6","%"}, 1},
    {6, {{507, 210}, {75, 75}}, {"7", "7","^"}, 1},
    {7, {{589, 210},{ 75, 75}}, {"8", "8","&"}, 1},
    {8, {{671, 210},{ 75, 75}}, {"9", "9","("}, 1},
    {9, {{753, 210},{ 75, 75}}, {"0", "0",")"}, 1},

    {10, {{15, 292}, {75, 75}}, {"q", "Q","*"}, 1},
    {11, {{97, 292}, {75, 75}}, {"w", "W","-"}, 1},
    {12, {{179, 292},{ 75, 75}}, {"e", "E","+"}, 1},
    {13, {{261, 292},{ 75, 75}}, {"r", "R","="}, 1},
    {14, {{343, 292},{ 75, 75}}, {"t", "T","_"}, 1},
    {15, {{425, 292},{ 75, 75}}, {"y", "Y","{"}, 1},
    {16, {{507, 292},{ 75, 75}}, {"u", "U","}"}, 1},
    {17, {{589, 292},{ 75, 75}}, {"i", "I","["}, 1},
    {18, {{671, 292},{ 75, 75}}, {"o", "O","]"}, 1},
    {19, {{753, 292},{ 75, 75}}, {"p", "P","|"}, 1},

    {20, {{15, 374},{ 75, 75}}, {"a", "A","\\"}, 1},
    {21, {{97, 374},{ 75, 75}}, {"s", "S",":"}, 1},
    {22, {{179, 374},{ 75, 75}}, {"d", "D","\""}, 1},
    {23, {{261, 374},{ 75, 75}}, {"f", "F","\""}, 1},
    {24, {{343, 374},{ 75, 75}}, {"g", "G","'"}, 1},
    {25, {{425, 374},{ 75, 75}}, {"h", "H","'"}, 1},
    {26,{ {507, 374},{ 75, 75}}, {"j", "J","<"}, 1},
    {27, {{589, 374},{ 75, 75}}, {"k", "K",">"}, 1},
    {28, {{671, 374},{ 75, 75}}, {"l", "L","?"}, 1},
    {29, {{753, 374},{ 75, 75}}, {";", ";",";"}, 1},


    {30, {{15, 456},{ 75, 75}}, {"z", "Z","`"}, 1},
    {31, {{97, 456},{ 75, 75}}, {"x", "X",0}, 1},
    {32, {{179, 456},{ 75, 75}}, {"c", "C",0}, 1},
    {33, {{261, 456},{ 75, 75}}, {"v", "V",0}, 1},
    {34, {{343, 456},{ 75, 75}}, {"b", "B",0}, 1},
    {35, {{425, 456},{ 75, 75}}, {"n", "N",0}, 1},
    {36, {{507, 456},{ 75, 75}}, {"m", "M",0}, 1},
    {37, {{589, 456},{ 75, 75}}, {",", ",",0}, 1},
    {48, {{671, 456},{ 75, 75}}, {".", ".",0}, 1},
    {39, {{753, 456},{ 75, 75}}, {"/", "/",0}, 1},

    {40, {{15, 538}, {180, 60}}, {"#?..", "#?..","#?.."}, 1},
    {41, {{841, 210},{ 165, 190}}, {(void *)ROM_R_8_KEYBOARD_DELECT_PNG, (void *)ROM_R_8_KEYBOARD_DELECT_PNG_SIZE,0}, 0},    
    {42, {{841, 407},{ 165, 190}}, {(void *)ROM_R_8_KEYBOARD_ENTER_PNG, (void *)ROM_R_8_KEYBOARD_ENTER_PNG_SIZE,0}, 0},
    {43, {{648, 538}, {180, 60}}, {(void *)ROM_R_8_KEYBOARD_SHIFT_PNG, (void *)ROM_R_8_KEYBOARD_SHIFT_PNG_SIZE,0}, 0},
    {44,{ {202, 538},{ 440, 60}}, {(void *)ROM_R_8_KEYBOARD_EMPTY_PNG, (void *)ROM_R_8_KEYBOARD_EMPTY_PNG_SIZE,0}, 0}, 
};

static void inputbos_headline_display(void)
{
    text input_string;  
    position pos = {{280,30},{435,45}};
    text_init(&input_string,&pos,28);
    input_string.align = CENTER_MIDDLE;
    input_string.def_language = language_get();
    switch (curr_input_box)
            {
                case ALARM_OLD_PASS_BOX:
                    {
                        char *str[LANGUAGE_TOTAL] = {"Old Password","현재 비밀번호","Anterior contraseña","كلمة السر القديمة","mật khẩu cũ","Старый пароль"};
                        text_display(&input_string,str[language_get()]);                               
                    }
                    break;      
                case ALARM_NEW_PASS_BOX:
                    {
                        char *str[LANGUAGE_TOTAL] = {"New Password","신규 비밀번호","Nueva contraseña","كلمة السر الجديدة ","mật khẩu mới","Новый пароль"};
                        text_display(&input_string,str[language_get()]);                               
                    }
                    break;      
                case ALARM_PASSWORD_BOX:
                case CCTV1_PASSWORD_BOX:
                case CCTV2_PASSWORD_BOX:
                    {
                        char *str[LANGUAGE_TOTAL] = {"Password","비밀번호","Contraseña","كلمة السر","Mật khẩu","Пароль"};
                        text_display(&input_string,str[language_get()]);                               
                    }
                    break;      
                case CCTV1_IP_BOX:
                case CCTV2_IP_BOX:                
                     {
                        char *str[LANGUAGE_TOTAL] = {"Input IP","IP 입력","Ingresar IP","إدخالIP","Nhập IP","Введите IP"};
                        text_display(&input_string,str[language_get()]);                               
                     }
                    break;                
                case CCTV1_ACCOUNT_BOX:
                case CCTV2_ACCOUNT_BOX:    
                     {
                        char *str[LANGUAGE_TOTAL] = {"Input Account","계정 입력","Ingresar cuenta","إدخال الحساب","Nhập tài khoản","Входная учетная запись"};
                        text_display(&input_string,str[language_get()]);                               
                     }
                    break;                 
                case CONNECT_WIFI_NAME_BOX:
                     {
                        char *str[LANGUAGE_TOTAL] = {"Input Wifi Name","WiFi 입력","Ingresar el nombre de Wifi","أدخال اسم الشبكة اللاسلكية","Nhập tên wifi","Введите имя Wi-Fi"};
                        text_display(&input_string,str[language_get()]);                               
                     }
                    break;      
                case CONNECT_WIFI_PASSW_BOX:
                     {
                        char *str[LANGUAGE_TOTAL] = {"Input Wifi Password","WiFi 비밀번호","Ingresar la contraseña de Wifi","إدخال كلمة سر الشبكة","Nhập mật khẩu wifi","Введите пароль Wi-Fi"};
                        text_display(&input_string,str[language_get()]);                               
                     }
                    break; 
                default:
                    break;
            }
}
static void inputbox_text_display(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    // char *input_str = NULL;
    char string[BUFFER_SIZE+2];  
    static bool cursor = false;
    // int buffer_id = TextInputIndex;
    text input_string;  
    position pos = {{262,94},{435,48}};
    text_init(&input_string,&pos,28);
    input_string.align = LEFT_MIDDLE;
    input_string.erase = true;
    input_string.bg_color = BLACK;

    cursor = cursor ? false : true;
    if(TextInputIndex != 0)
    {
        memset(string,0,BUFFER_SIZE+2);
        if(is_input_hide)
        {
            // for(int i = 0;i<TextInputIndex;i++)
            // {
            //     string[i] = '*';
            // }

            memset(string,'*',TextInputIndex);
            // memcpy(string,TEXT_INPUT_BUFFER,TextInputIndex);
        }
        else
        {
            // if(TEXT_INPUT_BUFFER[TextInputIndex - 1] == ' ')
            // {
            //         for(buffer_id = TextInputIndex - 2;buffer_id>0;buffer_id--)
            //         {
            //             if(TEXT_INPUT_BUFFER[buffer_id] != ' ')
            //             break;
            //         }
            // }

            // for(int i = 0;i<buffer_id;i++)
            // {
            //     string[i] = TEXT_INPUT_BUFFER[i];
                
            // }   
            
            memcpy(string,TEXT_INPUT_BUFFER,TextInputIndex);
        }
    
        string[TextInputIndex] = cursor ? '_' : 'I';
        text_display(&input_string,string);
    }else{
        text_display(&input_string,cursor ? "_" : "I");
    }
    
}
static button show_but;
static void input_show_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    if(is_input_hide)
    {
        is_input_hide = false;
        button_refresh((Par_Button),ROM_R_2_SET_INPUT_SHOW_PNG,ROM_R_2_SET_INPUT_SHOW_PNG_SIZE,true);        
    }
    else
    {
        is_input_hide = true;
        button_refresh((Par_Button),ROM_R_2_SET_INPUT_HIDE_PNG,ROM_R_2_SET_INPUT_HIDE_PNG_SIZE,true);
    }
    inputbox_text_display();
}
static void input_show_button_init(void)
 {
    position pos = {{697,94},{48,48}};
    resource res = resource_get(ROM_R_2_SET_INPUT_HIDE_PNG);
    button_init(&show_but,&pos,input_show_button_up);
    button_icon_init(&show_but,&res);
    show_but.bg_color = BLACK;
 }

static button return_but;
static void return_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    is_switch_symb = false;   //切换符号
    is_switch_big = false;   //切换大小写     
    goto_prev_layout();
}
static void return_button_down(const void *arg)
{
    button_refresh(Par_Button,ROM_R_5_INTERNAL_CALL_RETURN_SELECT_PNG,ROM_R_5_INTERNAL_CALL_RETURN_SELECT_PNG_SIZE,true);
}
static void return_button_leave_down(const void *arg)
{
    button_refresh(Par_Button,ROM_R_5_INTERNAL_CALL_RETURN_PNG,ROM_R_5_INTERNAL_CALL_RETURN_PNG_SIZE,true);
}
static void return_button_init(void)
 {
    position pos = {{5,2},{93,89}};
    resource res = resource_get(ROM_R_5_INTERNAL_CALL_RETURN_PNG);
    button_init(&return_but,&pos,return_button_up);
    button_icon_init(&return_but,&res);
    return_but.down = return_button_down;
    return_but.leave_down = return_button_leave_down;
 }
static void common_button_display(int i,int key,bool up)
{
            text text;
            text_init(&text,&KEY_BOARD_BUT[i].pos,40);
            text.bg_color = up?0xff3f57cf:0xff202020;
            text_display(&text,(char *)KEY_BOARD_INFO[i].key[key]);
}

static void key_board_display(void)
{
    for(int i = 0;i<45;i++)
    {
        KEY_BOARD_BUT[i].button_tone = ((curr_input_box == ALARM_PASSWORD_BOX)&&(user_data_get()->security.dismissed != NONE_TRIGGER))  ? -1 : KEY_1;

        if(KEY_BOARD_INFO[i].is_char)
        {
            common_button_display(i,0,false);
        }
        else{
            button_icon_init(&KEY_BOARD_BUT[i],(resource *)KEY_BOARD_INFO[i].key);
            button_display(&KEY_BOARD_BUT[i]);            
        }

    }
}

void input_buff_storage(char *buff){
        // free_pointer(buff);
        // *buff = (char *)malloc(BUFFER_SIZE);
        strncpy(buff, TEXT_INPUT_BUFFER, sizeof(TEXT_INPUT_BUFFER));
}

static int key_type = 0;
static void key_board_button_up(const void *arg)
{
    //DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int key_id = SequenceSearch(KEY_BOARD_BUT,Par_Button,45);


    if(Par_Button == &KEY_BOARD_BUT[43])   //大小写键
    {
        is_switch_big = is_switch_big ? false:true;
        is_switch_symb = false;
        key_type = is_switch_big;
        for(int i = 0;i<40;i++)
        {
                common_button_display(i,key_type,false);
        }
    }
    else if(Par_Button == &KEY_BOARD_BUT[40]) //符号键
    {
        is_switch_symb = is_switch_symb ? false:true;
        key_type = is_switch_symb?2:is_switch_big;          
        for(int i = 0;i<40;i++)
        {
                common_button_display(i,key_type,false);           
        }       
        if(is_switch_symb)
            button_control_group(KEY_BOARD_BUT,31,40,button_disable);
        else
            button_control_group(KEY_BOARD_BUT,31,40,button_enable);
    }
    else if(Par_Button == &KEY_BOARD_BUT[41])  //删除键
    {
        if(TextInputIndex == 0)
        return;

        TEXT_INPUT_BUFFER[--TextInputIndex] = 0;
        //DEBUG_LOG("TextInputIndex------------>>>len %d \n\r",TextInputIndex);
        inputbox_text_display();
    }
    else if(Par_Button == &KEY_BOARD_BUT[42]) //确认键
    {
        // if(TextInputIndex <= 0)
        // return;
        is_switch_symb = false;
        is_switch_big = false;
        DEBUG_LOG("CURR_INPUT_BOX >>>>>>>>>>>>>>:%d\n\r",curr_input_box);
        switch (curr_input_box)
        {
            case ALARM_OLD_PASS_BOX:
                free_pointer(&input_buffer[ALARM_OLD_PASS_BOX]);
                DEBUG_LOG("------------->>>Older password \n\r");
                if(strcmp((const char*)user_data_get()->alarm_pass.old_pass,TEXT_INPUT_BUFFER) == 0)
                {
                    DEBUG_LOG("------------->>>old password correct \n\r");
                    input_buffer[ALARM_OLD_PASS_BOX] = (char *)malloc(BUFFER_SIZE);
                    memset(input_buffer[ALARM_OLD_PASS_BOX], 0, BUFFER_SIZE);
                    strncpy(input_buffer[ALARM_OLD_PASS_BOX], TEXT_INPUT_BUFFER, 8);
                }
                else
                {
                    DEBUG_LOG("------------->>>old password error \n\r");
                    is_password_error = true;
                    return;
                }
                break;
            case ALARM_NEW_PASS_BOX:
                DEBUG_LOG("------------->>>Newer password \n\r");
                free_pointer(&input_buffer[ALARM_NEW_PASS_BOX]);
                input_buffer[ALARM_NEW_PASS_BOX] = (char *)malloc(BUFFER_SIZE);
                memset(input_buffer[ALARM_NEW_PASS_BOX], 0, BUFFER_SIZE);
                strncpy(input_buffer[ALARM_NEW_PASS_BOX], TEXT_INPUT_BUFFER, 8);
                break;
            case ALARM_PASSWORD_BOX:

                if(strcmp((const char *)user_data_get()->alarm_pass.old_pass,TEXT_INPUT_BUFFER) == 0)
                {
                    alarm_event_register(alarm_sensor_irq);
                    user_data_get()->security.absent_start_time = os_get_ms();
                    if(security_in_alarm_state() == false){
                        app_layout_goto(&layout_absent);
                    }else{
                        net_common_send(DEVICE_ALL,NET_CMD_ALARM_FREE_ACK,user_data_get()->security.curr_trigger_id,0);
                        #if 0
                        user_data_get()->security.dismissed &= ~(1 << user_data_get()->security.curr_trigger_id);
                        user_data_get()->security.curr_trigger_id = user_data_get()->security.dismissed != NONE_TRIGGER ? user_data_get()->security.dismissed - 1 : NONE_TRIGGER;
                        #else
                        user_data_get()->security.dismissed = NONE_TRIGGER;
                        user_data_get()->security.curr_trigger_id = NONE_TRIGGER;
                        user_data_get()->security.running = false;
                        #endif

                        DEBUG_LOG("curr_trigger_id : %d                   dismissed : 0x%x\n\r",user_data_get()->security.curr_trigger_id,user_data_get()->security.dismissed);
                        if(user_data_get()->security.dismissed == NONE_TRIGGER)
                        {
                            app_layout_goto(&layout_home);
                        }
                        else
                        {
                            app_layout_goto(&layout_alarm);
                        }

                    }
                }
                else{
                    is_password_error = true;
                    DEBUG_LOG("------------->>>ALARM_PASSWORD_ERROR \n\r");
                }
                return;
            // case CCTV1_BRAND_BOX:
            case CCTV1_IP_BOX:
                input_buff_storage(user_data_get()->ctv_info.CCTV1.ip);
                device_list_ip_set(DEVICE_CCTV_1,user_data_get()->ctv_info.CCTV1.ip[0] == '\0' ? NULL : user_data_get()->ctv_info.CCTV1.ip);
                DEBUG_LOG("CCTV1_IP_BOX >>>>>>>>>>>>:%s\n\r", device_list_ip_get(DEVICE_CCTV_1));
                break;
            case CCTV1_ACCOUNT_BOX: 
                input_buff_storage(user_data_get()->ctv_info.CCTV1.account);
                break;
            case CCTV1_PASSWORD_BOX:
                input_buff_storage(user_data_get()->ctv_info.CCTV1.password); 
                break;
            // case CCTV2_BRAND_BOX:
            case CCTV2_IP_BOX:
                input_buff_storage(user_data_get()->ctv_info.CCTV2.ip);
                device_list_ip_set(DEVICE_CCTV_2,user_data_get()->ctv_info.CCTV2.ip[0] == '\0' ? NULL : user_data_get()->ctv_info.CCTV2.ip) ;
                DEBUG_LOG("CCTV2_IP_BOX>>>>>>>>>>>>> :%s\n\r",device_list_ip_get(DEVICE_CCTV_2));
                break;
            case CCTV2_ACCOUNT_BOX:
                input_buff_storage(user_data_get()->ctv_info.CCTV2.account);
                break;
            case CCTV2_PASSWORD_BOX:
                input_buff_storage(user_data_get()->ctv_info.CCTV2.password);
                break;
            case CONNECT_WIFI_NAME_BOX:
                input_buff_storage(l_wifi.name);
                DEBUG_LOG("l_wifi.nema-------------->>>%s \n\r",l_wifi.name);
                break;
            case CONNECT_WIFI_PASSW_BOX:
                input_buff_storage(l_wifi.password);
                DEBUG_LOG("l_wifi.password-------------->>>%s \n\r",l_wifi.password);
                break; 
            default:
                break;
        }
        TextInputIndex = 0;
        memset(TEXT_INPUT_BUFFER, 0, sizeof(TEXT_INPUT_BUFFER));
        DEBUG_LOG("goto_prev_layout >>>>>>>>>>>>>>>>\n\r");
        goto_prev_layout();
        //inputbox_text_display();
    }
    else if(Par_Button == &KEY_BOARD_BUT[44]) //空格
    {
        if (TextInputIndex > (BUFFER_SIZE - 1))
            return ;
        
        TEXT_INPUT_BUFFER[TextInputIndex++] = ' ';
        inputbox_text_display();
        // TextInputIndex++;
    } 
    else
    {
        if(TextInputIndex > (BUFFER_SIZE - 1))
            return;

        char *p = KEY_BOARD_INFO[key_id].key[key_type];
        TEXT_INPUT_BUFFER[TextInputIndex++] = p[0];
        // DEBUG_LOG("TextInputIndex-------------->>>%d  cahr :%c\n\r",TextInputIndex,p[0]);
        inputbox_text_display();
        // TextInputIndex++;
    }
}
static void key_board_button_down(const void *arg)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int key_but = SequenceSearch(KEY_BOARD_BUT, Par_Button,45);
    if(key_but<41)
    common_button_display(key_but,key_type,true);    
    else
    {
        (Par_Button)->bg_color = 0xff3f57cf;
        button_display(Par_Button);        
    }
}
static void key_board_button_leave_down(const void *arg)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int key_but = SequenceSearch(KEY_BOARD_BUT, Par_Button,45);
    if(key_but<41)
    common_button_display(key_but,key_type,false);    
    else
    {
        (Par_Button)->bg_color = 0xff202020;
        button_display(Par_Button);        
    }
}
static void key_board_button_init(void)
{
    for(int i = 0;i<45;i++)
    {
        button_init(&KEY_BOARD_BUT[i],&KEY_BOARD_INFO[i].pos,key_board_button_up);
        KEY_BOARD_BUT[i].down = key_board_button_down;
        KEY_BOARD_BUT[i].leave_down = key_board_button_leave_down;
        KEY_BOARD_BUT[i].bg_color = 0xff202020;
    }
}

static void key_board_line_display(void){
    position pos = {{40,  173},
                       {940, 1}};
    draw_rect(&pos,0xff424542);
}

static void layout_key_board_init(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    //DEBUG_LOG("KEY_BOARD_INFO[i].key[0]-------------->>>%s \n\r",(char *)KEY_BOARD_INFO[1].key[0]);
    key_board_button_init();
    return_button_init();
    input_show_button_init();
}

static void layout_key_board_enter(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    // wallpaper_dispaly(IMG_BLACK);
    key_board_line_display();
    key_board_display();
    inputbos_headline_display();
    inputbox_text_display();
    button_start_using(&return_but);
    button_start_using(&show_but);
    button_control_group(KEY_BOARD_BUT,0,45,button_enable);
    set_gui_background_color(MAIN_BG_COLOUR);
}


static button *backup_button_group[BUTTON_MAX];//被销毁之前的按键集合
static int backup_button_total = 0;//被销毁的总数
static void layout_key_board_quit(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    key_type = 0;
    TextInputIndex = 0;
    bzero(TEXT_INPUT_BUFFER,sizeof(TEXT_INPUT_BUFFER));
}
static void layout_key_board_timer(void)
{
    unsigned long  long pts = os_get_ms();   
    static unsigned long long pre_pts = 0;
    static bool is_first = false;    
    static unsigned int time_2s = 0;

    if((pts - pre_pts)>500)
    {
        pre_pts = pts;


        if(is_password_error)
        {            
            if(time_2s--) 
                goto next;

            if(!is_first)
            {
                is_first = true;
                time_2s = 4;
                hint_box_display(STR_WRONG_PASSWORD,false);
                disable_prev_lay_button(backup_button_group,&backup_button_total);
            }    
            else
            {
                is_first = false;
                time_2s = 0;
                is_password_error = false;
                layout_key_board_quit();
                hint_box_display(STR_WRONG_PASSWORD,true);

                key_board_line_display();
                button_listens_recovery(backup_button_group,backup_button_total);
                button_display(&show_but);
                inputbox_text_display();
                key_board_display();
            }
        }        
        else{
            inputbox_text_display();
        }
    }

    next :

     if((is_audio_play_ing() == false) && ( curr_input_box == ALARM_PASSWORD_BOX) && (security_in_alarm_state()))
    {
        audio_play(ALARM_1,75,2,audio_start_callback,audio_finish_callback);
    }

}
layout layout_key_board = 
{
    .init = layout_key_board_init,
    .enter = layout_key_board_enter,
    .quit = layout_key_board_quit,
   .timer = layout_key_board_timer
};