#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H
#include "os_sys_api.h"

#define probe_line DEBUG_LOG("%s : %d --- \n\r",__FUNCTION__ ,__LINE__);
bool layout_switch;
extern layout layout_logo;
extern layout layout_home;
extern layout layout_set;
extern layout layout_door_cam;
extern layout layout_internal_call;
extern layout layout_capture;
extern layout layout_absent;
extern layout layout_alarm;
extern layout layout_key_board;
extern layout layout_sleep;
extern layout layout_tuya_key;
void layout_init(void);

bool get_layout_switch_flag();
layout *prev_layout;
layout *goto_layout;
#endif