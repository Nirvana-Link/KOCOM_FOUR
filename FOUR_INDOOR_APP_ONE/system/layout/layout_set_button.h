#ifndef  LAYOUT_SET_BUTTON_H
#define LAYOUT_SET_BUTTON_H
#include"ui_api.h"
#define Set_General        
#define Set_Sound           
//#define Set_Connect        
#define Set_Mode    
#define Set_Other        
#define Set_Reset   


button manually_button;
enum option_ack{
    RESET_YES_BUTTON,
    RESET_NO_BUTTON,
    OTHER_YES_BUTTON,
    OTHER_NO_BUTTON,
    CONNECT_YES_BUTTON,
    CONNECT_NO_BUTTON,
    NET_TIME_ACK_BUTTON,
    TUYA_CTRL_ACK_BUTTON,
    TUYA_CTRL_NO_BUTTON,
    OPTION_ACK_TOTAL
};
enum input_type{
    OTHER_OLD_PASSWORD,
    OTHER_NEW_PASSWORD,
    CONNECT_WIFI_NAME,
    CONNECT_WIFI_PASSWORD,
    INPUT_TYPE_TOTAL
};


enum QR_CODE{
    TUYA_QR,
    OFFICIAL_QR1,
    OFFICIAL_QR2,
    QR_BUTTON_TOTAL
};

enum input_show{
    OTHER_OLD_PASSW,
    OTHER_NEW_PASSW,
    CONNECT_WIFI_PASSW,
    INPUT_SHOW_TOTAL
};
button QR_CODE_BUTTON[QR_BUTTON_TOTAL];
button INPUT_SHOW_BUT[INPUT_SHOW_TOTAL];
button INPUT_BUT[INPUT_TYPE_TOTAL];
button OPTION_ACK[OPTION_ACK_TOTAL];
enum main_set_button{
    GENERAL_BUTTON,
    SOUND_BUTTON,
#ifdef Set_Connect 
    CONNECT_BUTTON,
#endif
    MODE_BUTTON,
#ifdef Set_Other
    OTHER_BUTTON,
#endif
    RESET_BUTTON,
    MAIN_SET_BUTTON_TOTAL
};

enum system_time{
    TIME_BUTTON,
    DATA_BUTTON,
    NET_SYN_BUTTON,
    SYS_TIME_TOTAL
};

enum device_button{
    DEVICE_1_BUTTON,
    DEVICE_2_BUTTON,
    DEVICE_3_BUTTON,
    DEVICE_4_BUTTON,
    DEVICE_BUTTON_TOTAL
};
enum language_button{
LANGUAGE_E_BUTTON,
LANGUAGE_K_BUTTON,
LANGUAGE_S_BUTTON,
LANGUAGE_A_BUTTON,
LANGUAGE_V_BUTTON,
LANGUAGE_R_BUTTON,
LANGUAGE_BUTTON_TOTAL
};
enum general_adjust{
    UP_D_BUTTON = 0,
    UP_M_BUTTON,
    UP_Y_BUTTON,
    DOWN_D_BUTTON,
    DOWN_M_BUTTON,
    DOWN_Y_BUTTON,

    UP_H_BUTTON,
    UP_MI_BUTTON,    
    DOWN_H_BUTTON,
    DOWN_MI_BUTTON,
    GENERAL_ADJUST_TOTAL
};

button tuya_app_button;
button MAIN_SET[MAIN_SET_BUTTON_TOTAL];
button SYSTEM_TIME[SYS_TIME_TOTAL];
button DEVICE[DEVICE_BUTTON_TOTAL];
button LANGUAGE[LANGUAGE_BUTTON_TOTAL];
button GENERAL_ADJUST[GENERAL_ADJUST_TOTAL];
  
enum sound_sour_switch{
    LEFT_SWITCH_BUTTON,
    RIGHT_SWITCH_BUTTON,
    SWITCH_BUTTON_TOTAL
};

enum sound_source_dev{
SOURCE_DEVICE,
SOURCE_CAM1,
SOURCE_CAM2,
SOURCE_DEV_TOTAL
};
enum mode_time_type{
    MONITORING_TIME,
    DOOR_TIME,
    TIME_TYPE_TOTAL
};
enum sound_adjust{
    CALL_VOL_SUB = 0,
    CALL_MEL_SUB,
    TALK_VOL_SUB,
    SPEAK_SEN_SUB,
    MIC_SEN_SUB,
    CALL_VOL_ADD,
    CALL_MEL_ADD,
    TALK_VOL_ADD,
    SPEAK_SEN_ADD,
    MIC_SEN_ADD,   
    SOUND_ADJUST_TOTAL
};

button SOUND_SOURCE[SWITCH_BUTTON_TOTAL];
button SOUND_ADJUST[SOUND_ADJUST_TOTAL];

enum mode_adjust{
    ABSENT_DELAY_SUB,
    MONITOR_SUB,
    DOOR_TIME_SUB,
    ABSENT_DELAY_ADD,
    MONITOR_ADD,
    DOOR_TIME_ADD,   
    MODE_ADJUST_TOTAL
};
enum switch_key{
    ALWAYS_NO = 0,
    AUTO_IMAGE,
    // NET_TIME,
#if SD_CARD
    AUTO_RECODE,   
#endif
    WIFI_POWER,
    SWITCH_TOTAL
};
button MODE_ADJUST[MODE_ADJUST_TOTAL];
button SWITCH_BUT[SWITCH_TOTAL];

enum other_set_key{
    BRIGHTNESS_SUB,
    BRIGHTNESS_ADD,
    SET_PASSWORD,
    DEVICE_SET,
    DEVICE_INFORMATION,
    MANUAL_DOWNLOAD,
    UPGRADE_OUTDOOR,

    CCTV1_SET,
    CCTV2_SET,

    BRAND_SET,
    IP_SET,
    ACCOUNT,
    CCTV_PASSWORD,
    OTHER_SET_TOTAL
};
enum return_button_key{
    PASSWORD_RETURN,
    DEVICE_RETURN,
    UPGRADE_RETURN,

    CCTV_RETURN,
#ifdef Set_Connect 
    CONNECT_RETURN,
#endif
    RESET_TOTAL
};

enum other_upgrade{
    UPGRADE_OUTDOOR1,
    UPGRADE_OUTDOOR2,
    OTHER_UPGRADE_TOTAL
};

button RETURN_BUTTON[RESET_TOTAL];
button OTHER_SET[OTHER_SET_TOTAL];
button OTHER_UPGRADE[OTHER_UPGRADE_TOTAL];
#endif