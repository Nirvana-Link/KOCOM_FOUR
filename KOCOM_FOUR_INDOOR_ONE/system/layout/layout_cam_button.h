#ifndef LAYOUT_CAM_BUTTON_H
#define LAYOUT_CAM_BUTTON_H
#include"ui_api.h"



//#define Set_PQ
icon rolling_ball;
button attr_open_button;
enum cam_set_button{
#ifdef Set_PQ
    EXTEND_BUTTON,
    EXTEND_CLOSE_BUTTON,
    EXTEND_REFRESH_BUTTON,
#endif
    LOCAL_VOL_ADJUST_BUTTON,
    LOCAL_BRI_ADJUST_BUTTON,
    OUTDOOR_VOL_ADJUST_BUTTON,
#ifdef Set_PQ
    CON_ADJUST_BUTTON,
    SAT_ADJUST_BUTTON,
    SHA_ADJUST_BUTTON,
    HUE_ADJUST_BUTTON,
#endif
    CAM_SET_TOTAL
};
enum cam_action_button{
    ACTION_CAM1_BUTTON,
    ACTION_CAM2_BUTTON,
    ACTION_CCTV1_BUTTON,
    ACTION_CCTV2_BUTTON,
    ACTION_HOME_BUTTON,
    ACTION_ANSWER_BUTTON,
    ACTION_DOOR2_BUTTON,
#if SD_CARD
    ACTION_RECODE_BUTTON,
#endif
    // ACTION_DOOR3_BUTTON,    
    ACTION_SHOOT_BUTTON,
    ACTION_HANGUP_BUTTON,
    ACTION_SET_TOTAL
};
button CAM_SET[CAM_SET_TOTAL];
button CAM_ACTION[ACTION_SET_TOTAL];


#endif