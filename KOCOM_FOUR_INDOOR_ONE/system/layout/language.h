#ifndef LANGUAGE_H
#define LANGUAGE_H
#include"ui_api.h"

#define RED                       (0xFFEC1C24)
#define ORANGEREB      (0xFFBD4848)
#define YELLOW               (0xFFF20000)
#define BLUE                     (0xBB2962ff)
#define GREEN                 (0x0ED14500)
#define WHITE                  (0xFFFFFFFF)
#define BLACK                  (0xFF000000)
#define LUCE_BLACK                  (0x00000000)
#define GRAY                     (0xFF727572)/* (0xFF727572) */
#define BG_COLOR            (0xff292d36)
#define TRANSLUCENCE    (0x80000000)
#define DEEP_TRANSPARENT    (0xCB000000)
#define VERY_TRANSPARENT    (0XF2000000)
#define MAIN_BG_COLOUR               (0x292e37)
enum time_type{
    CLOCK_TIME,
    DATE_TIME,
    WEEK_TIME,
    PERIOD_TIME,
    TOTAL_TIME
};

enum direction{
    NOT = 0,
    ANSWER,
    DOOR2
};

enum set_option{
    BRIGHTNESS = 0,
    CONTRAST,
    SATURATION,
    HUE,
    SHARPNESS,
    MUSIC
};

enum font_size{
    HOME_BUTTON,
    HOME_DATE_STR,
    HOME_DATE,
    INTERCOM_BUTTON,
    HEADLINE,
    HOUSE_DEVICE,
    CAM_DOOR,
    CAM_DOOR_SET,
    SET_OPTION,
    SET_OPTION_INFO,
    SET_RESET_OPTION,
    CAPTURE_BUTTON,
    CAPTURE_INFO,
    ALARM_INFO,
    ABSENT_BUTTON,
    ABSENT_INFO,
    SIZE_MAX
};
enum language_type{
    ENGLISH = 0,
    KOREAN,
    SPANISH,
    ARABIC,
    VIETNAMESE,
    RUSSIAN,
    LANGUAGE_TOTAL
};
enum btn_string_id{
    STR_0 = 0,
    STR_1,
    STR_2,
    STR_3,
    STR_4,
    STR_DOOR_CAM,
    STR_INTERCOM,
    STR_NEW_CAPTURE,
    STR_DISTURB,
    STR_ABSENT,
    STR_SET,
    STR_MON,
    STR_TUE,
    STR_WED,
    STR_THUR,
    STR_FRI,
    STR_SAT,
    STR_SUN,
    STR_ALL,
    STR_DEVICE_ID,
    STR_DEVICE,
    STR_DEVICE_CONFLICT,
    STR_DEVICE1,
    STR_DEVICE2,
    STR_DEVICE3,
    STR_DEVICE4,
    STR_PM,
    STR_AM,
    STR_DOOR_OPENED,
    STR_RESET,
    STR_YES,
    STR_NO,
    STR_ISSUE,
    STR_GENERAL,
    STR_SOUND,
    STR_CONNECT,
    STR_MODE,
    STR_OTHER,
    STR_LANGUAGE,
    STR_ENGLISH,
    STR_KOREAN,
    STR_SPAIN,
    STR_RUSSIAN,
    STR_ARAB,
    STR_VIETNAMESE,   
    STR_TIME,
    STR_DATE,
    STR_INTERPHONE,
    STR_CALL_VOLUME,
    STR_TALK_VOLUME,
    STR_CALL_MELODY,
    STR_SPEAL_SEN,
    STR_MIC_SEN,
    STR_ALWAYS_ON,
    STR_AUTO_IMAGE,
    STR_NET_TIME,//*
    STR_AUTO_RECODE,   //WQS_ADD
    STR_BRIGHTNESS,   //WQS_ADD
    STR_ABSENT_DELAY,   //WQS_ADD
    STR_MONITORING,
    STR_DOOR_TIME,      //WQS_ADD
    STR_SET_PASSWORD,//WQS_ADD
    STR_OLD_PASSWORD,//WQS_ADD
    STR_NEW_PASSWORD,//WQS_ADD
    STR_SET_DEVICE,//WQS_ADD
    STR_SENSOR1,      //WQS_ADD
    STR_SENSOR2,      //WQS_ADD
    STR_CAM1,      //WQS_ADD
    STR_CAM2,      //WQS_ADD
    STR_CCTV1,      //WQS_ADD
    STR_CCTV2,      //WQS_ADD
    STR_BRAND,      //WQS_ADD
    STR_IP,      //WQS_ADD
    STR_ACCOUNT,      //WQS_ADD
    STR_PASSWORD,      //WQS_ADD
    STR_MANUALLY,      //WQS_ADD
    STR_WIFI_NAME,      //WQS_ADD
    // STR_HIK,      //WQS_ADD
    // STR_DAH,      //WQS_ADD
    STR_CAM_CONFLICT,      //WQS_ADD
    STR_CAM_DISCONNECT,      //WQS_ADD
    STR_NO_SIGNAL,      //WQS_ADD
    STR_CAM_BUSY,      //WQS_ADD
    STR_NO_DEVICE,      //WQS_ADD
    STR_ABSNET_START,      //WQS_ADD
    STR_ABSNET_STOP,      //WQS_ADD
    STR_ACTION_NOW,      //WQS_ADD
    STR_DEVICE_INFO,      //WQS_ADD
    STR_UPGRADE_OUTDOOR,      //WQS_ADD
    STR_UPGRADE_OUTDOOR1,      //WQS_ADD
    STR_UPGRADE_OUTDOOR2,      //WQS_ADD
    STR_QUIT,
    STR_NOTICE,
    STR_TUYA_NET_TIME_TEXT1,
    STR_TUYA_NET_TIME_TEXT2,
    STR_MODIFY_SUCCESSFULLY,
    STR_DELETE_ALL_PHOTO,
    STR_DELETE_PHOTO,
    STR_DELETE_ALL_EVENT,
    STR_WRONG_PASSWORD,
    STR_WAIT_ISSUE,
    STR_MANUAL,
    STR_AUTO_SET,
    STR_PHONE,
    STR_TUYA_OPEN_HINT1,
    STR_TUYA_OPEN_HINT2,
    STR_TUYA_CLOSE_HINT,
    STR_DEVICE_BUSY,
    STR_INTERNAL,
    STR_DUPLICATE,
    STR_APPROVAL,
    STR_BACK,
    STR_NONE,
    STR_TOTAL
};
const char *multi_lingual[STR_TOTAL][LANGUAGE_TOTAL];
void* btn_str(enum btn_string_id str_id);
const char* text_str(enum btn_string_id str_id);
int font_size(enum font_size type);
#endif