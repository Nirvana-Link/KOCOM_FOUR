#ifndef LAYOUT_INTERNAL_BUTTON_H
#define LAYOUT_INTERNAL_BUTTON_H
#include"ui_api.h"

enum device_internal_status{
    IDLE_STATUS,
    LINK_STATUS,
    CALL_STATUS,
    TALK_STATUS,
    ALL_STATUS
};


enum call_device_button{
    DEVICE1_CALL_BUTTON,
    DEVICE2_CALL_BUTTON,
    DEVICE3_CALL_BUTTON,
    DEVICE4_CALL_BUTTON,
    // CALL_ALL_BUTTON,    
    DEVICE_CALL_ALL 
};
button CALL_DEVICE[DEVICE_CALL_ALL];
#endif