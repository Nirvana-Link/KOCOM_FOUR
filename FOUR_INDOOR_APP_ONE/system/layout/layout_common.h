#ifndef LAYOUT_COMMON_H
#define LAYOUT_COMMON_H
#include"ak_common.h"
#include"layout_base.h"
#include"l_user_config.h"
#include"language.h"
#include"ui_api.h"
#include"rom.h"
#include"file_api.h"

#define PRESST_RIGGER
#define Par_Button                           ((button *)arg)
typedef enum {
    IMG_BLACK,
    SET,
    CALL_BG,
    ALARM_RED,
    ALARM_BLUE,
    KEY_BOARD,
    CLOCK_1,
    CLOCK_2,
    CLOCK_3,
    CLOCK_4,
    CLOCK_5,
    TUYA_RUN,
    WALL_PAPER_TOTAL
}wallpaper_type;
bool sd_curr_status;
button goto_home_button;
void hint_box_display(enum btn_string_id str_id,bool need_erasure);
void alarm_sensor_irq(int ev,int arg1);
#if SD_CARD
bool delect_sdcard(bool layout_jump);
#endif
void date_set(struct ak_date *date);
void get_network_time(void);
bool is_leap_feb(struct ak_date date);
int layout_get_week(struct ak_date *date);
bool photo_file_all_clear(int photo_type);
void home_date_time_display(struct ak_date *date,enum btn_string_id id,position pos,enum time_type type);
void layout_get_LocalDate(struct ak_date *date,int *week);
void get_time_str(char *time,int len);
void add_absent_event(int sour_id,int dev_id);
void disable_listens_button(button**pbtn_group,int *index);
void enable_listens_button(button**pbtn_group,int *index);
void count_display( int conut,bool is_erase);
void goto_home_button_display(bool normal);
void goto_home_button_init(void);
void button_refresh(button *Button,long int res,long int res_size,bool is_erase);
bool button_start_using(const button *Button);
bool button_start_disable(const button *Button);
extern int abs(int );
int SequenceSearch(button *but, button *Button , int total);
void button_control_group(button *but,int start,int end,bool (*control)(const button*));
void wallpaper_dispaly(wallpaper_type res_type);
void free_pointer(char **p);
char * mid(char *dst,char *src, int n,int m);
void app_layout_goto(layout* layout);
void goto_prev_layout(void);
bool is_point_in_scope(const point* point,const position* rect);
void disable_prev_lay_button(button** pbtn_group,int* index);
void enable_prev_lay_button(button** pbtn_group,int* index);
int echo(char *value,char *path);
int output_file_use(char *cata);
void Audio_play_stop(void);
void confirm_interface(enum btn_string_id str_id1,enum btn_string_id str_id2,bool need_erasure,unsigned int color,int font_size);
void headline_display(bool head_display);
void clock_display(int x,int y);
void language_text_display(text *text,const char* str,enum language language);
bool button_tone_mute(const button*BUT);
bool button_tone_open(const button*BUT);
bool return_disturb_status(void);
#endif