#ifndef L_CCTV_INFO_H
#define L_CCTV_INFO_H
#include"stdio.h"
#include"stdbool.h"

#define CCTV_HIK        true
#define CCTV_DAH       false
typedef struct{
    bool CCTV_BRAND;
    char ip[18];
    char account[18];
    char password[18];
    char url[128];
}CCTV_INFO;

typedef struct 
{
    CCTV_INFO CCTV1;
    CCTV_INFO CCTV2;
}CCTV_DEV;

// CCTV_DEV ctv;
// extern enum source_index;
void rtsp_video_open(int cctv);
#endif