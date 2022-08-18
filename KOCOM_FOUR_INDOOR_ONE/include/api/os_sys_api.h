#ifndef OS_SYS_API_H
#define OS_SYS_API_H
#include <stdbool.h>
#include "ak_common.h"
#include"network_function_api.h"


typedef struct
{
    void(*init)(void);

    void(*enter)(void);

    void(*quit)(void);

    void(*timer)(void);
}layout;





unsigned long long os_get_ms(void);
unsigned long long os_get_us(void);
int os_get_week_form_date(struct ak_date*date);

/********************
页面跳转接口
********************/
void os_layout_goto(const layout* layout);

/********************
投递按键事件发送
********************/
bool touch_key_event(int key_index,int key_state);

/***********
初始化主任务
************/
void os_evnet_init(void);


/******************
开启主任务工作
******************/
void os_start(const layout* layout);

/*************************************************************************************
开启定时待机
	timeout:超时待机，单位毫秒，<= 0，沿用上次的事件，
	timeout_callback:超时处理函数,timeout_callback = NULL,使用上一次的超时处理函数
**************************************************************************************/
bool standby_timer_open(int timeout,void(*timeout_callback)(void));


/********************
关闭超时处理。
********************/
bool standby_timer_close(void);


/********************
待机超时重置
********************/
bool standby_timer_reset(void);

/*************************************************************************************
开启定时设备查询
	timeout:超时查询，单位毫秒，<= 0，沿用上次的事件，
	timeout_callback:超时处理函数,timeout_callback = NULL,使用上一次的超时处理函数
**************************************************************************************/
bool dev_lookup_open(int time_out,void (*func)(unsigned int,unsigned int,unsigned int));
bool looup_time_set_param(unsigned int arg1,unsigned int arg2,unsigned int update);
void looup_dufault_param1_set(unsigned int parem);
void looup_dufault_param2_set(unsigned int parem);

/********************
关闭超时处理。
********************/
bool dev_lookup_close(void);


bool timed_event_check_init(void(*timed_check_func)(void));
void timed_event_check_start(void);
void timed_event_check_stop(void);

/********************
投递记录事件发送
********************/
bool record_finish_event(char type,bool is_finish);
/****************************
记录 事件处理函数的注册
*****************************/
void record_event_register(void(*callback)(char,bool));

/********************
投递interphone事件
********************/
bool interphone_event(int arg1,int arg2);
/****************************
interphone 事件处理函数的注册
*****************************/
void interphone_event_register(void(*callback)(int,int));


/*******************
call事件处理函数注册
********************/
void door_call_event_register(void(*door1_func)(void),void(*door2_func)(void));


void internal_call_event_register(void(*func1)(int,int));

/********************
hook事件处理函数注册
********************/
void hook_event_register(void(*hook_up)(void),void(*hook_down)(void));



/*******************
警报事件处理函数注册
********************/
void alarm_event_register(void(*func)(int,int));
void alarm_free_event_register(void(*func)(int));



/*******************
同步事件处理函数注册
********************/
void syn_event_register(void(*func)(unsigned int  ,unsigned int ,unsigned int ,char *data ));

typedef enum
{
	/*切换监控*/
	TUYA_EVENT_MONITOR_SWAP,

	/*开锁*/
	TUYA_EVENT_OPEN_DOOR,

    /*Absent mode*/
    TUYA_EVENT_ABSENT_MODE,
	
	/*通话*/
	TUYA_EVENT_TALK,

	/*进入监控*/
	TUYA_EVENT_MONITOR_ENTER,

	/*退出监控*/
	TUYA_EVENT_MONITOR_QUIT,

	/*涂鸦定义*/
	TUYA_EVENT_INIT_OPEN
	
}tuya_event;

/*
*	涂鸦事件发送与处理
*/
void tuya_event_register(bool(*func)(unsigned int ,unsigned int));

void tuya_monitor_swap_event(int ch);

void tuya_monitor_talk_event(void);

void tuya_monitor_unlock_event(bool state);

void tuya_monitor_enter_event(void);

void tuya_monitor_quit_event(void);

void tuya_monitor_absent_mode_event(bool state);

void tuya_init_open_event(network_device dev);

/*
*	网络事件发送与处理
*/
typedef enum
{
	/* 门口机在线心跳应答 */
	NETWORK_EVENT_OUTDOOR_ACK,

	/* 室内机与门口机通话 */
	NETWORK_EVENT_OUTDOOR_TALK,

	/* 室内机与门口机忙线 */
	NETWORK_EVENT_OUTDOOR_BUYS,

	/* 室内机与门口机空闲 */
	NETWORK_EVENT_OUTDOOR_LEISURE,

	/* 室内机与门口机连线 */
	NETWORK_EVENT_OUTDOOR_LINK,

	/* 室内机之间通话 */
	NETWORK_EVENT_INTERNAL_TALK,

	/* 室内机之间通话挂断 */
	NETWORK_EVENT_INTERNAL_CANCEL,

	/* 室内机忙线 */
	NETWORK_EVENT_INTERNAL_BUSY,

	/* 室内机空闲可响铃 */
	NETWORK_EVENT_INTERNAL_RING,

	/* 室内机ID冲突 */
	NETWORK_EVENT_INTERNAL_CONFLICT,

	/* 时间同步应答 */
	NETWORK_EVENT_TIME_SYN_ACK,

	/* 门锁同步应答 */
	NETWORK_EVENT_UNLOCK_SYN_ACK,

	/* CCTV信息同步请求 */
	NETWORK_EVENT_CCTV_SYN_REQUEST,

	/* CCTV信息同步应答 */
	NETWORK_EVENT_CCTV_SYN_ACK,

	/* 警报启用控制 */
	NETWORK_EVENT_ALARM_CONTROL,

	/* 警报触发响应 */
	NETWORK_EVENT_ALARM_TRIGGER,

	/* 警报结束响应 */
	NETWORK_EVENT_ALARM_FREE,

	/* 警报密码同步 */
	NETWORK_EVENT_PASSWORD_SYNC,
}network_event;
void network_event_regiter(bool(*func)(unsigned int ,unsigned int,unsigned int));

void network_outdoor_talk_event(network_device dev);

void network_outdoor_event(network_event event,int dev_id,int arg3);

void network_internal_event(network_event event,network_device dev,int arg3);

void network_syn_event(network_event event,unsigned char arg1,unsigned char arg2,char *data);
#endif

