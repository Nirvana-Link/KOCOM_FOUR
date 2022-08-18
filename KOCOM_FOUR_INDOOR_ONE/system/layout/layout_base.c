#include"layout_base.h"

bool layout_switch = true;
void layout_init(void)
{
    layout_logo.init();
    layout_home.init();
    layout_set.init();
    layout_alarm.init();
    layout_door_cam.init();    
    layout_internal_call.init();
    layout_capture.init();
    layout_absent.init();
    layout_key_board.init();   
    layout_sleep.init();
}

bool get_layout_switch_flag(void)
{
    bool flag = layout_switch;
    layout_switch = false;
    return flag;
}