#ifndef TUYA_SDK_H
#define TUYA_SDK_H
#include <stdbool.h>
#include  "tuya_cloud_types.h"

#define KOCOM_DP_DOORBELL             (136)
#define KOCOM_DP_LOCK                 (148)
#define KOCOM_DP_PICTURE              (154)
#define KOCOM_DP_ALARM_MSG            (185)
#define KOCOM_DP_SWITCH_CHANNEL       (231)
#define KOCOM_DP_ABSENT_MODE          (232)
#define KOCOM_DP_DEVICE_ACTIVE        (233)
#define KOCOM_DP_ABNORMAL_UNLOCK      (234)



bool tuya_wifi_sdk_init(const char* pid,char *uuid,char *key);

bool tuya_sdk_init_status(void);

int is_tuya_cloud_connected_num(void);

int tuya_dp_232_response_absent_mode(BOOL_T state);
#endif

