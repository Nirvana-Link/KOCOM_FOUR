#ifndef UI_API_H
#define UI_API_H
#include <stdbool.h>
#include "ak_vo.h"
#include "rom.h"

typedef struct
{
    int x;
    int y;
}point;

typedef struct 
{
    int width;
    int height;
}vector;

typedef struct
{
    point point;
    vector vector;
}position;

typedef struct 
{
    long id;
    int size;
}resource;


typedef enum
{
    LEFT_TOP,
    LEFT_MIDDLE,
    LEFT_BOTTOM,

    CENTER_TOP,
    CENTER_MIDDLE,
    CENTER_BOTTOM,

    RIGHT_TOP,
    RIGHT_MIDDLE,
    RIGHT_BOTTOM
}align;


/***********************************************************
vector:LCD的屏幕分辨率
font_resource:字体资源的相关信息
button_down_pro_func:触摸按下，或者按键按下的处理函数,一般用
					于播放按键音，重置待机时间。
touch_x_scale,touch_y_scale:触摸按下坐标进行百分比比例缩放。
***********************************************************/
void ui_init(vector* vector,const resource* font_resource,void(*button_down_pro_func)(int arg),int touch_x_scale,int touch_y_scale,char rota,bool touch_swap_xy);

bool draw_rect(const position* rect,unsigned int color);

bool gui_erase(const position* pos,unsigned int color);

bool bg_erase(const position* pos,unsigned int color);

void ui_layer_display(void);

void screen_display(void);

void screen_update_enable(void);

void screen_update_disable(void);
#define resource_get(x) {x,x##_SIZE}

enum language{
    language_english = 0,
    language_korean,
    language_spanish,
    language_arabic,
    language_vietnamese,
    language_russian,
    language_chinese,
    language_total
};

//icon
typedef struct
{
    position pos;
    // position frame;
    point offset;
    resource res;
    unsigned int bg_color;

    align align;

    bool erase;

    bool png_buffer;
    
    enum ak_vo_layer layer;
}icon;

bool icon_init(icon* icon,const position* pos,const resource* res);
bool icon_display(const icon*icon);

//text
typedef struct
{
    int font_size;
    unsigned int font_color;
    unsigned int bg_color;

    align align;
    position pos;
    // position frame;
    point offset;
    bool erase;
    enum language def_language;
}text;

bool text_init(text* text,const position* pos,int font_size);
bool text_display(text*text,const char* string);


#define BUTTON_MAX 64

#define TOUCH_STATE_UP          0X01
#define TOUCH_STATE_DOWN        0X02
#define TOUCH_STATE_MOTION      0X03
#define TOUCH_STATE_LONG_DOWN   0X04
#define TOUCH_STATE_SHORT_DOWN   0X05

typedef enum{
	KEY_INDEX_1 = 0,
	KEY_INDEX_2,	
	KEY_INDEX_3,
	KEY_INDEX_4,
	KEY_INDEX_5,
	KEY_INDEX_TOTAL
}KEY_INDEX;


typedef enum
{
	KEY_STATE_UP = 0,
	KEY_STATE_DOWN,
	KEY_STATE_LONG_DOWN,
	KEY_STATE_TOTAL
}KEY_STATE;

typedef void(*touch_func_callback)(const void* arg);
typedef void(*touch_slide_func_callback)(const char arg);



typedef struct 
{
    position pos;
    // position frame;
    unsigned int bg_color;
    bool erase;
    bool png_buffer;
    bool key_enable;
    align icon_align;
    point icon_offset;
    resource resource;


    int font_size;
    int font_color;
    int button_tone;
    align font_align;
    point font_offset;

    enum language def_language;
    void* string;


    touch_func_callback down;
	touch_func_callback leave_down;
    touch_func_callback up;
    touch_func_callback long_down;
    touch_func_callback short_down;
    touch_func_callback motion;
}button;

bool button_init(button* btn,const position* pos,touch_func_callback up);

bool button_icon_init(button*btn,resource* resource);

bool button_text_init(button*btn,void* string,int font_size);

bool button_display(const button* btn);

bool button_enable(const button* pbtn);

bool button_disable(const button* ptbn);

bool button_listens_copy(button** pbtn_group,int* index);

bool button_listens_recovery(button **pbtn_group,int index);

bool button_listens_destroy(void);

bool button_touch_event_handle(char state,const point* point);

void language_set(enum language lang );

enum language language_get(void);

bool bg_resource_load(const resource* res,const position* src_pos,const point* offset,align align);

bool jpeg_resource_load(const resource* res,const position* pos);

bool gui_background_copy_to_cache(void);

bool gui_background_recovery_from_cache(void);

bool set_gui_background_color(unsigned int color);

bool menu_back_ground_color(void);

bool gui_background_clear(void);

bool gui_background_cache_destroy(void);

const unsigned char* gui_backgound_cache_addres_get(void);

void png_buffer_init(void);
void png_buffer_destroy(void);

bool key_touch_handle_register(KEY_INDEX key_index,KEY_STATE key_state,void(*key_func)(void));

/*********************************
down_callback(void* arg)
motion_callback(void* arg)
arg为point*类型
*********************************/
enum touch_slide_direction{
    INVALID_MOVE,
    SLIDE_LEFT,
    SLIDE_RIGHT,
    SLIDE_UP,
    SLIDE_DOWN
};

void touch_other_down_register(touch_func_callback down_callback);
void touch_slide_callback_register(touch_slide_func_callback slide_callback);

/*********************************
当前按下的绝对坐标点
*********************************/
const point* touch_down_point_get(void);

const point* touch_current_point(void);


typedef struct
{
	resource bg_res; //时钟的背景资源
	resource hour_res; //时钟的时针资源
	resource min_res;
	resource sec_res;
	resource dot_res;//时钟的中心原点（可以省略）

	int hour_center_y;//距离时钟的垂直距离点旋转
	int min_center_y;
	int sec_center_y;

	position pos;//显示位置
}analog_clock;


bool analog_clock_init(const analog_clock* clock_info);
void set_clock_dot_offset(point offset_xy);
bool analog_clock_update(void);
bool analog_clock_deinit(void);
void set_clock_dot_offset(point offset_xy);


int compare_two_rectangles(position pos1,position pos2);

#endif
