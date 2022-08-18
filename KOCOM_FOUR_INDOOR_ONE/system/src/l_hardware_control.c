#include"l_hardware_control.h"
#include <network_function_api.h>
#include"tuya_sdk_api.h"
#include"l_monitor.h"
#include"l_user_config.h"
#include "stdio.h"
#include <unistd.h>
#include "string.h"
#include "fcntl.h"
#include"debug.h"

#define HARDWARE_PRINT (printf("\n[hardware controller] line:%d ",__LINE__),printf)

#define IO_DOOR_OPEN 85

void gpio_detect_interupt_init(void)
{
    isr_gpio_info info;
    info.door1_call_pin = -1;
    info.door2_call_pin = -1;
    info.hook_pin = -1;
    info.alarm_det_pin_group[0] = IO_SENSOR1_LEVEL_DETECT;
    info.alarm_trigger_level_group[0] = GPIO_LEVEL_HIGH;
    info.alarm_det_pin_group[1] = IO_SENSOR2_LEVEL_DETECT;
    info.alarm_trigger_level_group[1] = GPIO_LEVEL_HIGH;
    info.alarm_det_pin_num = 2;

    bool ret = gpio_isr_init(&info);

    HARDWARE_PRINT("device gpio interrupt init: [%s]\n",
                   ret == false ? "\033[41m FAIL \033[40m " : "SUCCESS"
                   );
}

GPIO_LEVEL sensor_gpio_level_get(const int pin)
{
    GPIO_LEVEL level;
    gpio_read(pin,&level);
    return level;
}

static bool door_opened_flag = false;

bool door_status_get(void) {
    return door_opened_flag;
}

// void door_control(network_device device,bool open) {
//     door_opened_flag = open;
//     tuya_dp_148_response_accessory_lock(open);
//     if(open){
//         unlock_send_cmd(device,user_data_get()->user_default_mode_conf.time_conf.door_time,user_data_get()->user_default_device);

//         //if(user_data_get()->security.running){
//         //    tuya_dp_234_response_abnormal_unlock();
//         //}
//     }
//     HARDWARE_PRINT("DOOR LOCK:%s [%s]   %d\n",
//                    open ? " DOOR OPEN " : "DOOR CLOSE",
//                    open == false ? "\033[41m FAIL \033[40m " : "SUCCESS",
//                    user_data_get()->user_default_mode_conf.time_conf.door_time
//         );
// }

void door_control(network_device device, bool open) {

    bool ret = gpio_set(IO_DOOR_OPEN, (open ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW));
    if (ret) {
        door_opened_flag = open;
        tuya_dp_148_response_accessory_lock(open);
        if (open) {
            unlock_send_cmd(device,user_data_get()->user_default_mode_conf.time_conf.door_time,user_data_get()->user_default_device);
            if(user_data_get()->security.running){
                tuya_dp_234_response_abnormal_unlock();
            }
        }
    }
    HARDWARE_PRINT("DOOR LOCK:%s [%s]\n",
                   open ? " DOOR OPEN " : "DOOR CLOSE",
                   ret == false ? "\033[41m FAIL \033[40m " : "SUCCESS"
        );
}

static unsigned long long start_unlock_ms = 0;

void start_door_unlock(void){

    start_unlock_ms = os_get_ms();
    enum source_index ch = monitor_channel_get();
    if(ch == CAM1){
        door_control(DEVICE_OUTDOOR_1,true);
    }else if(ch == CAM2){
        door_control(DEVICE_OUTDOOR_2,true);
    }
}
bool wait_door_unlock_finish(void){

    if(door_status_get() == false){
        return true;
    }
    unsigned long long delay = os_get_ms() - start_unlock_ms;

    if((user_data_get()->user_default_mode_conf.time_conf.door_time == 0) && (delay >= 200)){
        door_control(DEVICE_ALL, false);
        DEBUG_LOG("unlock finish delay:%llu \n\r",delay);
        return true;
    }else if(delay > (user_data_get()->user_default_mode_conf.time_conf.door_time*1000)){
        door_control(DEVICE_ALL,false);
        DEBUG_LOG("unlock finish delay:%llu \n\r",delay);
        return true;
    }
    return false;
}


int bl_pwm_duty_set(const int pwm,unsigned int duty_cycle,unsigned int period){

    char path[64] = {0};
    char buffer[64];
    int fd = -1;
    int len = 0;
    sprintf(&path[0], "/sys/class/pwm/pwmchip0/pwm%d", pwm);
    if(access(path, F_OK) != 0){
        memset(path, 0, 64);
        sprintf(&path[0], "/sys/class/pwm/pwmchip0/export");
        fd = open(path, O_WRONLY);
        if (fd < 0){
            DEBUG_LOG("\"%s\" open failed \n", path);
            return -1;
        }


        len = snprintf(buffer, sizeof(buffer), "%d", pwm);
        if (write(fd, buffer, len) < 0){
            DEBUG_LOG("write failed to export pwmd%d!\n",pwm);
        }
        close(fd);
    }

    memset(path,0,sizeof(path));
    memset(buffer,0,sizeof(buffer));
    sprintf(&path[0], "/sys/class/pwm/pwmchip0/pwm%d/enable",pwm);
    fd = open(path, O_WRONLY);
    if (fd < 0){
        DEBUG_LOG("\"%s\" open failed \n", path);
        return -1;
    }

    memset(buffer,0,sizeof(buffer));
    len = snprintf(buffer, sizeof(buffer), "%d", duty_cycle?1:0);
    if (write(fd, buffer, len) < 0){
        DEBUG_LOG("write failed to pwmd%d - duty enable : %d!\n",pwm,duty_cycle);
    }
    close(fd);

    if(duty_cycle == 0){
        return 0;
    }


    memset(path,0,sizeof(path));
    sprintf(&path[0], "/sys/class/pwm/pwmchip0/pwm%d/period",pwm);
    fd = open(path, O_WRONLY);
    if (fd < 0){
        DEBUG_LOG("\"%s\" open failed \n", path);
        return -1;
    }

    memset(buffer,0,sizeof(buffer));
    len = snprintf(buffer, sizeof(buffer), "%d", period);
    if (write(fd, buffer, len) < 0){
        DEBUG_LOG("write failed to pwmd%d - period : %d!\n",pwm,period);
    }
    close(fd);

    memset(path,0,sizeof(path));
    sprintf(&path[0], "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle",pwm);
    fd = open(path, O_WRONLY);
    if (fd < 0){
        DEBUG_LOG("\"%s\" open failed \n", path);
        return -1;
    }

    memset(buffer,0,sizeof(buffer));
    len = snprintf(buffer, sizeof(buffer), "%d", duty_cycle);
    if (write(fd, buffer, len) < 0){
        DEBUG_LOG("write failed to pwmd%d - duty cycle : %d!\n",pwm,duty_cycle);
    }
    close(fd);
    return 0;
}



void set_lcd_brightness(int brightness){
    // DEBUG_LOG("set_lcd_brightness ---------------------》   BL:%d\n\r",brightness);
    const unsigned int duty[11] = {1,6553,13106,19659,26212,32756,39318,45871,52424,58977,65530};
    bl_pwm_duty_set(0,duty[brightness < 10? brightness : 10],65536);
}