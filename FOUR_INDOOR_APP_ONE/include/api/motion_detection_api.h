#ifndef MOTION_DETECTION_API_H
#define MOTION_DETECTION_API_H
#include <stdbool.h>

bool motion_detection_open(int sensivity,unsigned char threshold);

bool motion_detection_check(void);

bool motion_detection_close(void);

#endif
