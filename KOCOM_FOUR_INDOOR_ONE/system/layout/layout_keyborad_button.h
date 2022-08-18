#ifndef LAYOUT_KEYBOARD_BUTTON_H
#define LAYOUT_KEYBOARD_BUTTON_H
#include"ui_api.h"

typedef struct 
{
    int id;
    position pos;
    void *key[3];
    bool is_char;
}keyboard_info;

enum keyboard_inputbox{
  ALARM_OLD_PASS_BOX = 0,
  ALARM_NEW_PASS_BOX,
  CONNECT_WIFI_NAME_BOX,
  CONNECT_WIFI_PASSW_BOX,
  ALARM_PASSWORD_BOX,  
  // CCTV1_BRAND_BOX,
  CCTV1_IP_BOX,
  CCTV1_ACCOUNT_BOX,
  CCTV1_PASSWORD_BOX,
  // CCTV2_BRAND_BOX,
  CCTV2_IP_BOX,
  CCTV2_ACCOUNT_BOX,
  CCTV2_PASSWORD_BOX,
  INPUTBOX_TOTAL
};


enum keyboard_inputbox curr_input_box;
char *input_buffer[5];
#endif