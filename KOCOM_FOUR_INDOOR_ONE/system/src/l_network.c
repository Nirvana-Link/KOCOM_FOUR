#include"l_network.h"
#include"network_function_api.h"
#include"l_hardware_control.h"

static unsigned int net_recv_dev;
static unsigned int net_send_cmd;
static unsigned int net_send_arg1;
static unsigned int net_send_arg2;
static unsigned char *net_send_data1;
static unsigned int net_send_data2;
static unsigned int net_send_count;

void lookup_dev_status(unsigned int arg1,unsigned int arg2,unsigned int update)
{
    send_lookup_device_cmd(arg1,arg2,update);
    wait_door_unlock_finish();
}

void network_cmd_param_set(unsigned int revc_dev,unsigned int send_cmd,unsigned int send_arg1,unsigned int arg2,unsigned int send_count){
        net_recv_dev = revc_dev;
        net_send_cmd = send_cmd;
        net_send_arg1 = send_arg1;
        net_send_arg2 = arg2;
        net_send_count = send_count;
}

bool network_cmd_send_start(void){
    if(net_send_count > 0){
        net_send_count --;
        net_common_send(net_recv_dev,net_send_cmd,net_send_arg1,net_send_arg2);
        return true;
    }
    return false;
}

void network_data_param_set(unsigned int revc_dev,unsigned int send_cmd,unsigned char *data,unsigned int size,unsigned int send_count){
        net_recv_dev = revc_dev;
        net_send_cmd = send_cmd;
        net_send_data1 = data;
        net_send_data2 = size;
        net_send_count = send_count;
}

bool network_data_send_start(void){
    if(net_send_count > 0){
        net_send_count --;
        net_common_data_send(net_recv_dev,net_send_cmd,net_send_data1,net_send_data2);
        return true;
    }
    return false;
}
