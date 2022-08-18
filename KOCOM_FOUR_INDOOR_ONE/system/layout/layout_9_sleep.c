#include"ui_api.h"
#include"l_user_config.h"
#include"l_hardware_control.h"
#include"gpio_api.h"
#include"debug.h"

static unsigned int prev_min = 0;
static unsigned int prev_sec = 0;
static unsigned int prev_day = 0;

button wakeup_button;
position sleep_posD = {{300,448},{24,31}};       //年月日
position sleep_posW = {{198,480},{214,50}};        //周
position sleep_posHM = {{436,430},{50,109}};        //时分
position sleep_posP = {{677,472},{114,60}};        //时段
icon sec_point;
static void sec_point_icon_init(void)
{
    position pos = {{775,190},{13,49}};
    resource res =resource_get(ROM_R_1_HOME_I_PNG);
    icon_init(&sec_point,&pos,&res);
    // HMS_i.png_buffer = true;
}

void wakeup_func(const void *arg){
    // DEBUG_LOG("wakeup------------->>>succeed \n\r");
    app_layout_goto(&layout_home);
    //button_disable(&wakeup);
}
void wakeup_button_init(void)
{
    position pos = {{0,0},{1024,600}};
    button_init(&wakeup_button,&pos,wakeup_func);
}
static void layout_sleep_init(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    wakeup_button_init();
    sec_point_icon_init();
}

static void layout_sleep_enter(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    prev_min = 0;
    prev_sec = 0;
    prev_day = 0;
    wallpaper_dispaly(IMG_BLACK);
    button_enable(&wakeup_button);
    if(user_data_get()->user_default_mode_conf.user_default_always)
    {
        analog_clock_deinit();
        clock_display(355,70);        
    }

}

static void layout_sleep_quit(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    set_lcd_brightness(user_data_get()->user_default_mode_conf.brightness);
}

static void layout_sleep_timer(void)
{
    int week;
    static wallpaper_type wall_count = CLOCK_1;
    layout_get_LocalDate(&user_data_get()->user_default_time,&week);
    if(!user_data_get()->user_default_mode_conf.user_default_always && get_layout_switch_flag()){
        set_lcd_brightness(0);
        gui_background_clear();
        return;
    }else if(user_data_get()->user_default_mode_conf.user_default_always){
        if(user_data_get()->user_default_time.day != prev_day || layout_switch == true){
            prev_day = user_data_get()->user_default_time.day;
            home_date_time_display(&user_data_get()->user_default_time,(week+STR_MON),sleep_posD,DATE_TIME);
            home_date_time_display(&user_data_get()->user_default_time,(week+STR_MON),sleep_posW,WEEK_TIME);            
        }

        if(user_data_get()->user_default_time.minute != prev_min || layout_switch == true){
            wall_count = wall_count < CLOCK_5 ?  wall_count + 1: CLOCK_1;
            // DEBUG_LOG("wall_count:%d   prev_min:%ld   user_data_get()->user_default_time.minute : %ld\n\r",wall_count,prev_min,user_data_get()->user_default_time.minute);
            prev_min = user_data_get()->user_default_time.minute;
            wallpaper_dispaly(wall_count);
            analog_clock_update();
            home_date_time_display(&user_data_get()->user_default_time,((user_data_get()->user_default_time.hour<12)?STR_AM:STR_PM),sleep_posP,PERIOD_TIME);            
        }
        
        if(user_data_get()->user_default_time.second != prev_sec || layout_switch == true){
            prev_sec = user_data_get()->user_default_time.second;
            home_date_time_display(&user_data_get()->user_default_time,((user_data_get()->user_default_time.hour<13)?STR_AM:STR_PM),sleep_posHM,CLOCK_TIME);            
        }
        layout_switch = false;
    }
}
layout layout_sleep = 
{
    .init = layout_sleep_init,
    .enter = layout_sleep_enter,
    .quit = layout_sleep_quit,
   .timer = layout_sleep_timer
};