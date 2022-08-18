#ifndef JPEG_ENCODEC_API_H
#define JPEG_ENCODEC_API_H
#include <stdbool.h>

bool jpeg_encodec_start(const char* file_path,void(*finish_callback)(void));
bool check_jpeg_encodec_run(void);

#define TUYA_MSG_DOORBELL 0XDD
#define TUYA_MSG_ALARM1    0XA1
#define TUYA_MSG_ALARM2    0XA2

void jpg_push_to_tuya(int type);
bool network_jpeg_encodec_start(const char* file_path,void(*finish_callback)(void),int phy_id);
bool check_network_jpeg_encodec_run(void);

bool network_jpg_push_to_tuya_start(void);
#endif
