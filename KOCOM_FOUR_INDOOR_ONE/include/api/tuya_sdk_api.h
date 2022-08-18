#ifndef TUYA_SDK_API_H
#define TUYA_SDK_API_H
#include <stdbool.h>
#include  "tuya_cloud_types.h"
#include"tuya_ipc_api.h"
#include <time.h>

void tuya_ipc_ring_buffer_video_release_data(void);

bool tuya_wifi_sdk_init(const char* pid,char *uuid,char *key);

bool is_online_tuya_cloud(void);

int is_tuya_cloud_connected_num(void);

int tuya_dp_148_response_accessory_lock(BOOL_T state);

int tuya_dp_234_response_abnormal_unlock(void);

int tuya_dp_uploads_security_msg(char id , char *data,int size);

int tuya_switch_channel_upload_results(int channel);

void Monitor_channel_set(int ch);

bool tuya_net_time_sync(struct tm *local_time);

IPC_REGISTER_STATUS tuya_get_app_register_status(void);
#endif
