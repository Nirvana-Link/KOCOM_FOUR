#ifndef L_NETWORK_H
#define L_NETWORK_H
#include <stdio.h>
#include"layout_common.h"
#include"network_function_api.h"
// void get_locat_ip(char *ip,char *eth);

void lookup_dev_status(unsigned int arg1,unsigned int arg2,unsigned int update);

void network_cmd_param_set(unsigned int revc_dev,unsigned int send_cmd,unsigned int send_arg1,unsigned int arg2,unsigned int send_count);

bool network_cmd_send_start(void);

void network_data_param_set(unsigned int revc_dev,unsigned int send_cmd,unsigned char *data,unsigned int size,unsigned int send_count);

bool network_data_send_start(void);
#endif