#ifndef _SENSOR_VIDEO_API_H
#define _SENSOR_VIDEO_API_H
#include <stdbool.h>
#include "ui_api.h"
#include "ak_vi.h"

#define SENSOR_VIN_NONE -1
#define SENSOR_VIN_1 0
#define SENSOR_VIN_2 1
#define SENSOR_VIN_3 2
#define SENSOR_VIN_4 3

typedef bool (*sensor_capture_callback)(const struct video_input_frame*,const vector*);

/*************************************
reset_callback:复位IO口
det_callback:设置信号输入通道函数
write_callback:写入参数后处理函数

bright_map/cont_map/color_map 
亮度/对比度/色度的数字索引表/
比如
	bright_map[6][20] = 
	{
		{0x01,0x02,0x03,0x04....}, //表示cvbs pal的0-19的亮度的索引值
		{0x01,0x02,0x03,0x04....}, //表示cvbs ntsc的0-19的亮度的索引值
		{0x01,0x02,0x03,0x04....}, //表示AHD 720p 25fps的0-19的亮度的索引值
		{0x01,0x02,0x03,0x04....}, //表示AHD 720p 30fps的0-19的亮度的索引值
		{0x01,0x02,0x03,0x04....}, //表示FHD 1080P 25fps的0-19的亮度的索引值
		{0x01,0x02,0x03,0x04....}, //表示FHD 1080P 30fps的0-19的亮度的索引值
	};
map_size:调节范围，例如上面，map_size = 20(0-19).

注意。如果不想通过外部修改，可以将其全部写NULL即可（map_size = 0）。

默认：调节范围值为0-20。
**************************************/
bool sensor_video_init(void(*reset_callback)(void),
								int(*det_channel_callback)(void),
								bool(*write_cfg_callback)(int*,int*, int*),
								unsigned char(*bright_map)[6],
								unsigned char(* cont_map)[6],
								unsigned char(*color_map)[6],
								int map_size);

bool sensor_video_open(void);

bool sensor_video_close(void);

bool video_capture_open(void);

bool video_capture_close(void);

bool sensor_video_check(void);

void set_video_revice_status(bool status);
bool get_video_revice_status(void);
/*过滤前面帧数*/
void video_capture_filter_frames(int frames);


/********************************
调整画面显示。其中数值范围未0-20
*********************************/
void display_bright_adj(int bright);
void display_const_adj(int cont);
void display_color_adj(int color);





#endif

