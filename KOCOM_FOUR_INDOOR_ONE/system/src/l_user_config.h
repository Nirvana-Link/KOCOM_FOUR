#ifndef L_USER_CONFIG_H
#define L_USER_CONFIG_H
#include"layout_set_button.h"
#include"ak_common.h"
#include"layout_common.h"
#include"string.h"
#include"language.h"
#include"l_cctv_conf.h"
#include"network_function_api.h"

#define IPC_APP_PID   "h5dbwdxqdhibnmna"
#define USER_DATA_PATH "/etc/config/user_data.cfg"
#define FACTORY_CONF_FILE_PATH  "/etc/config/factory.txt"
#define DISABLE               0
#define ENABLE                1
#define standby_time    30000

#define SD_CARD             DISABLE
typedef struct
{
    int call_vol;
    int call_mel;
    int talk_vol;
    int speak_sen;
    int mic_sen;
}sound_property;
typedef struct
{
    int mon_time;
    int door_time;
}mode_time_index;
enum month{
  JAN,
  FEB,
  MAR,
  APR,
  MAY,
  JUN,
  JUL,
  AUG,
  SEP,
  OCT,
  NOV,
  DEC,
  MONTH_TOTAL
};

enum source_index{
  DEV, /*  此处并不表示输入源，仅为了调节sound时方便初始化音量的属性以及在配置monitor输入源时，起到与MONITOR_CHANNEL枚举对其的作用且不会作user_default_sour的值来使用*/
  CAM1,
  CAM2,
  CCTV1,
  CCTV2,
  SOURCE_TOTAL
};

typedef struct 
{
  enum source_index index;
  unsigned int bri;    
  sound_property sound;
  unsigned int outdoor_call_vol;    
  unsigned int outdoor_talk_vol;    
  unsigned int version;
}source_property;

typedef struct 
{
  bool user_default_always; 
  bool user_default_snap; 
  bool user_default_net_time; 
  bool user_default_record; 
  unsigned int user_default_absent_delay; 
  unsigned int brightness;
  mode_time_index time_conf;
}mode_config;


typedef enum{
    NONE_TRIGGER = 0x00,
    SENSOR1_TRIGGER = 0x01,
    SENSOR2_TRIGGER = 0x02,
    SENSOR_ALL_TRIGGER = 0x03,
}sensor_trigger_num;
typedef struct 
{
  bool Sensor1_open;
  bool Sensor2_open;
  unsigned char dismissed;
  bool running;
  unsigned long long absent_start_time;
  int curr_trigger_id;
  network_device curr_trigger_device;
}Security;

typedef struct 
{
  char old_pass[16];
  char new_pass[16];
}Alarm_password;

typedef struct 
{
    // char alarm_password[16];  //  64
    Alarm_password alarm_pass;
    CCTV_DEV ctv_info;
    enum language_type user_default_language;    
    network_device user_default_device;  //4
    struct ak_date user_default_time;    //28
    // sound_property SOUR_SOUND[SOURCE_DEV_TOTAL];    //120 
    mode_config user_default_mode_conf; 
    enum source_index user_default_sour;
    source_property SOUR_PRO[SOURCE_TOTAL];
    Security security;
    bool factory_reset;
    bool tuya_open;
    bool do_not_disturb;
}l_user_config;



l_user_config user_conf;

int get_process_pid(void);
void Password_updated(void);
void Save_config(char *param_file);
void user_configure_init(void);
l_user_config *user_data_get(void);
void  user_data_restore(void);
bool security_in_alarm_state(void);
#endif