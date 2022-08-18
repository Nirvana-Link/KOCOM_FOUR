#ifndef L_MONITOR_H
#define L_MONITOR_H
#include"network_function_api.h"
#include"ui_api.h"
#include"l_user_config.h"

typedef enum buys{
    NONE_STATUS,
    IDLE_STATUS,
    BUSY_STATUS
}buys_status;

typedef enum cam_buy{
    CAM1_BUY,
    CAM2_BUY,
    BUY_TOTAL
}buy_dev;



typedef struct {
    char connectable[BUY_TOTAL];
}BUY_CAMERA;

BUY_CAMERA CAMERA_STATUS;
#define MONITOR_ENTER_NONE	 0X00
#define MONITOR_ENTER_MANUAL 0x01
#define MONITOR_ENTER_CALL	 0X02
#define MONITOR_ENTER_TUYA   0X03

void monitor_doorbell(bool enable);
bool monitor_doorbell_status(void);
char monitor_enter_flag_get(void);
bool get_monitor_open_flag(void);
void monitor_enter_flag_set(char flag);
bool monitor_channel_set(enum source_index ch );
enum source_index monitor_channel_get(void);
bool monitor_open(void);
bool monitor_switch(int channel,bool audio_close);
bool monitor_close(void);
#endif