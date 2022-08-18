#include"os_sys_api.h"
#include"layout_base.h"
#include "stdio.h"
#include"ui_api.h"
#include"rom.h"
#include"layout_common.h"
#include"l_network.h"
#include"network_function_api.h"
#include "leo_tuya_key_check.h"
#include "tuya_sdk.h"
#include <time.h>
#include<stdlib.h>
#include"debug.h"

static int goto_layout_home(void)
{
    app_layout_goto(&layout_home);
    return 0;
}

void layout_logo_init(void)
{

}


void layout_logo_enter(void)
{
    DEBUG_LOG("%s --------------->>>%d\n\r",__func__,__LINE__);
    // display_logo();
    // syn_timing_query();
    if(tuya_key_uuid_check() == false){
        os_layout_goto(&layout_tuya_key);
            DEBUG_LOG("%s --------------->>>%d\n\r",__func__,__LINE__);
    }
    else
    {
            DEBUG_LOG("%s --------------->>>%d\n\r",__func__,__LINE__);
        if(user_data_get()->tuya_open == true)
        {
            tuya_open_device_set(user_data_get()->user_default_device);
            tuya_wifi_sdk_init(IPC_APP_PID,"tuya48603d9f736f3164","AQGxkOaXZp7G4MMJxFrfLTRPxSfrcEXz");
        }
            
        DEBUG_LOG("SECURITY.DISMISSED  ==================>>%d\n\r",user_data_get()->security.dismissed);
        // if(user_data_get()->security.dismissed != NONE_TRIGGER && user_data_get()->security.curr_trigger_device == user_data_get()->user_default_device)
        // {
        //     app_layout_goto(&layout_alarm);
        // }
        // else
        {
            goto_layout_home();
        }
    }
}
void layout_logo_timer(void)
{

    unsigned long  long pts = os_get_ms();   
    static unsigned long long pre_pts = 0;
    static bool is_first = false;

    if((pts - pre_pts)>2000)
    {

        if(is_first == false){
            is_first = true;
        }else{
            if(tuya_key_uuid_check() == false){
                os_layout_goto(&layout_tuya_key);
            }else{
                tuya_wifi_sdk_init(IPC_APP_PID,"tuya48603d9f736f3164","AQGxkOaXZp7G4MMJxFrfLTRPxSfrcEXz");
                goto_layout_home();
            }

        }
        pre_pts = pts;
    }
}


void layout_logo_quit(void)
{
    tuya_dp_232_response_absent_mode(user_data_get()->security.running);
}

layout layout_logo = 
{
    .init = layout_logo_init,
    .enter = layout_logo_enter,
    .quit = layout_logo_quit,
    // .timer = layout_logo_timer
};