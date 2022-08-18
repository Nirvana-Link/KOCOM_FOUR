#include"l_user_config.h"
#include"l_absent_ev.h"
#include"stdio.h"
#include"stdlib.h"
#include"unistd.h"
#include<unistd.h>
#include"layout_keyborad_button.h"
#include"debug.h"

#define CONF_ERROR (printf("\n[user configure] line:%d ",__LINE__),printf)

layout *prev_layout = &layout_home;
layout *goto_layout = &layout_home;
char *input_buffer[5] = {NULL};

l_user_config user_conf = {
        .alarm_pass.old_pass = {'0','0','0','0'},
        .alarm_pass.new_pass = {0},

        .ctv_info.CCTV1.CCTV_BRAND = CCTV_HIK,
        .ctv_info.CCTV1.ip = {0},
        .ctv_info.CCTV1.account = {0},
        .ctv_info.CCTV1.password = {0},
        .ctv_info.CCTV1.url = {0},
        .ctv_info.CCTV2.CCTV_BRAND = CCTV_HIK,
        .ctv_info.CCTV2.ip = {0},
        .ctv_info.CCTV2.account = {0},
        .ctv_info.CCTV2.password = {0},
        .ctv_info.CCTV2.url = {0},

        .user_default_language = ENGLISH,
        .user_default_device = DEVICE_INDOOR_ID1,
        .user_default_time = {2022,7,7,0,0,0,0},

        .user_default_mode_conf.user_default_always = true,
        .user_default_mode_conf.user_default_snap = false,
        .user_default_mode_conf.user_default_net_time = false,
        .user_default_mode_conf.user_default_record = false,
        .user_default_mode_conf.user_default_absent_delay = 60,
        .user_default_mode_conf.brightness = 6,
        .user_default_mode_conf.time_conf.mon_time = 30,
        .user_default_mode_conf.time_conf.door_time = 0,

        .user_default_sour = CAM1,

        .SOUR_PRO[DEV].index = DEV,
        .SOUR_PRO[DEV].bri = 9,
        .SOUR_PRO[DEV].sound = {4,5,4,1,1},

        .SOUR_PRO[CAM1].index = CAM1,
        .SOUR_PRO[CAM1].bri = 9,
        .SOUR_PRO[CAM1].sound = {4,1,4,1,1},
        .SOUR_PRO[CAM1].outdoor_call_vol = 4,
        .SOUR_PRO[CAM1].version = 0,

        .SOUR_PRO[CAM2].index = CAM2,
        .SOUR_PRO[CAM2].bri = 9,
        .SOUR_PRO[CAM2].sound = {4,2,4,1,1},
        .SOUR_PRO[CAM2].outdoor_call_vol = 4,
        .SOUR_PRO[CAM2].version = 0,

        .SOUR_PRO[CCTV1].index = CCTV1,
        .SOUR_PRO[CCTV1].bri = 6,
        
        .SOUR_PRO[CCTV2].index = CCTV2,
        .SOUR_PRO[CCTV2].bri = 6,
        
        .security.Sensor1_open = false,
        .security.Sensor2_open = false,
        .security.dismissed = NONE_TRIGGER,
        .security.running = false,
        .security.absent_start_time = 0,
        .security.curr_trigger_id = 2,
        .security.curr_trigger_device = DEVICE_ALL,

        .factory_reset = false,
        .tuya_open = false,
        .do_not_disturb = false,
};

void user_param_init(l_user_config *user_conf){
    // DEBUG_LOG("user_param_init >>>>>>>>>>>>>>>>>>>.factory_reset : %s\n\r ",user_conf->factory_reset == true ? "true": "false");
    // DEBUG_LOG("user_conf->user_default_language>>>>>>>>>>>>>>>>>>>%d\n\r ",user_conf->user_default_language);
    l_user_config user_param = {
        .alarm_pass.old_pass = {'0','0','0','0'},
        .alarm_pass.new_pass = {0},

        .ctv_info.CCTV1.CCTV_BRAND = CCTV_HIK,
        .ctv_info.CCTV1.ip = {0},
        .ctv_info.CCTV1.account = {0},
        .ctv_info.CCTV1.password = {0},
        .ctv_info.CCTV1.url = {0},
        .ctv_info.CCTV2.CCTV_BRAND = CCTV_HIK,
        .ctv_info.CCTV2.ip = {0},
        .ctv_info.CCTV2.account = {0},
        .ctv_info.CCTV2.password = {0},
        .ctv_info.CCTV2.url = {0},

        .user_default_language = ENGLISH,
        .user_default_device = DEVICE_INDOOR_ID1,
        .user_default_time = {2022,7,7,0,0,0,0},

        .user_default_mode_conf.user_default_always = true,
        .user_default_mode_conf.user_default_snap = false,
        .user_default_mode_conf.user_default_net_time = false,
        .user_default_mode_conf.user_default_record = false,
        .user_default_mode_conf.user_default_absent_delay = 60,
        .user_default_mode_conf.brightness = 6,
        .user_default_mode_conf.time_conf.mon_time = 30,
        .user_default_mode_conf.time_conf.door_time = 0,

        .user_default_sour = CAM1,

        .SOUR_PRO[DEV].index = DEV,
        .SOUR_PRO[DEV].bri = 9,
        .SOUR_PRO[DEV].sound = {4,5,4,1,1},

        .SOUR_PRO[CAM1].index = CAM1,
        .SOUR_PRO[CAM1].bri = 9,
        .SOUR_PRO[CAM1].sound = {4,1,4,1,1},
        .SOUR_PRO[CAM1].outdoor_call_vol = 4,
        .SOUR_PRO[CAM1].version = 0,

        .SOUR_PRO[CAM2].index = CAM2,
        .SOUR_PRO[CAM2].bri = 9,
        .SOUR_PRO[CAM2].sound = {4,2,4,1,1},
        .SOUR_PRO[CAM2].outdoor_call_vol = 4,
        .SOUR_PRO[CAM2].version = 0,

        .SOUR_PRO[CCTV1].index = CCTV1,
        .SOUR_PRO[CCTV1].bri = 6,

        .SOUR_PRO[CCTV2].index = CCTV2,
        .SOUR_PRO[CCTV2].bri = 6,
        
        .security.Sensor1_open = false,
        .security.Sensor2_open = false,
        .security.dismissed = NONE_TRIGGER,
        .security.running = false,
        .security.absent_start_time = 0,
        .security.curr_trigger_id = 2,
        .security.curr_trigger_device = DEVICE_ALL,

        .factory_reset = false,
        .tuya_open = false,
        .do_not_disturb = false,
    };
    
    if(user_conf->factory_reset == true){
        user_param.user_default_language = user_conf->user_default_language;
        user_param.security = user_conf->security;
        *user_conf = user_param;
    }
    user_data_get()->security.dismissed = NONE_TRIGGER;
    user_conf->security.absent_start_time = os_get_ms();
    user_conf->user_default_mode_conf.user_default_net_time = false;
    device_list_ip_set(DEVICE_CCTV_1,user_data_get()->ctv_info.CCTV1.ip[0] == '\0' ? NULL : user_data_get()->ctv_info.CCTV1.ip);
    device_list_ip_set(DEVICE_CCTV_2,user_data_get()->ctv_info.CCTV2.ip[0] == '\0' ? NULL : user_data_get()->ctv_info.CCTV2.ip);
}

int get_process_pid(void)
{
    return getpid();
}
void Password_updated(void)
{
    DEBUG_LOG("%s-------------------------->%s\n\r",user_data_get()->alarm_pass.old_pass,__func__);
    // free_pointer(&(user_data_get()->alarm_pass.old_pass));
    // user_data_get()->alarm_pass.old_pass= (char *)malloc(16);
    strncpy(user_data_get()->alarm_pass.old_pass, input_buffer[ALARM_NEW_PASS_BOX], 16);
}
void Save_config(char *param_file)
{
    FILE *fp = NULL;
    // l_user_config config;
    fp = fopen(param_file,"w+");    
    if(fp ==NULL)
    {
        CONF_ERROR("write open %s fail.\n\r",param_file);
        fclose(fp);
        return;
    }
    fwrite(&user_conf,sizeof(user_conf),1,fp);
    rewind(fp);
    // fread(&config,sizeof(config),1,fp);
    // DEBUG_LOG("config.user_default_device = %d\n\r",config.user_default_device);
    fclose(fp);
    return;
}
void user_configure_init(void){
    FILE *fp = NULL;
    // system("rm -f "FACTORY_CONF_FILE_PATH); 
    if(access(FACTORY_CONF_FILE_PATH,F_OK) != 0){
        DEBUG_LOG("--------------->>>FACTORY INEXISTENCE \n\r");
        // fp = fopen(FACTORY_CONF_FILE_PATH,"w");
        // if(fp == NULL){
        //     CONF_ERROR("open %s fail.\n\r",FACTORY_CONF_FILE_PATH);
        //     } else{
        //     fclose(fp);
        // }
        Save_config(FACTORY_CONF_FILE_PATH);        
    }
    else{
        DEBUG_LOG("--------------->>>FACTORY ALREADY EXIST \n\r");
        fp = fopen(FACTORY_CONF_FILE_PATH,"r");
        if(fp ==NULL)
        {
            CONF_ERROR("read open %s fail.\n\r",FACTORY_CONF_FILE_PATH);
            fclose(fp);
            return;
        }
        fread(&user_conf,sizeof(user_conf),1,fp);
    }
    user_param_init(&user_conf);
    language_set(user_data_get()->user_default_language);
    // date_set(&user_data_get()->user_default_time);
    absent_event_init(&absent_event_group);
}

l_user_config *user_data_get(void) {
    return &user_conf;
}
void  user_data_restore(void){
    //    system("rm -f "FACTORY_CONF_FILE_PATH); 

        // system("rm -f "ABSENT_EV_FILE_PATH); 
        photo_file_all_clear(FILE_TYPE_FLASH_PHOTO);
        user_data_get()->factory_reset = true;
        Save_config(FACTORY_CONF_FILE_PATH);
        DEBUG_LOG("user_data_get()->language:%d\n\r",user_data_get()->user_default_language);
       char temp[16] = {0};
    //    sprintf(temp,"kill -9 %d; app_run.sh",get_process_pid());
        sprintf(temp,"reboot");
        system(temp);
}

bool security_in_alarm_state(void) {
        if (user_data_get()->security.dismissed != NONE_TRIGGER) {
            return true;
        }
    return false;
}