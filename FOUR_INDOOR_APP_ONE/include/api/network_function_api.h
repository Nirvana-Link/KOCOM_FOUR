#ifndef NETWORK_FUNCTION_API_H
#define NETWORK_FUNCTION_API_H
#include <stdbool.h>
typedef enum
{
	DEVICE_INDOOR_ID1 = 0,
	DEVICE_INDOOR_ID2,
	DEVICE_INDOOR_ID3,
	DEVICE_INDOOR_ID4,
	DEVICE_OUTDOOR_1,
	DEVICE_OUTDOOR_2,
	DEVICE_CCTV_1,
	DEVICE_CCTV_2,
	DEVICE_TOTAL,

	DEVICE_ALL = 0XFF
}network_device;

typedef enum
{
	STREAM_NONE = 0,
	RECEIVE_STREAM,
	SEND_STREAM,
	STREAM_TOTAL,
}AUDIO_STREAM_TYPE;

typedef enum
{
	SOCK_NONE = 0,
	SOCK_SERVER,
	SOCK_CLINET,
}NET_SOCK_TYPE;

typedef enum
{
	DEVICE_IP_GET = 0,
	ONLINE_NUM_GET,
	INDOOR_CONFLICT_GET,
	OUTDOOR_CONFLICT_GET,
	DEVICE_STATUS_UPDATE_GET,
}DEVICE_STATUS_TYPE;


typedef enum
{
	NONE_BUSY_STATE = 0xff,
	CAMERA_BUSY_STATE = 1,
	INTERCOM_BUSY_STATE,
	ALARM_BUSY_STATE,
	DEVICE_BUSY_STATE_TOTAL
}DEVICE_BUSY_TYPE;

typedef struct
{
	network_device device;

	char ip[64];

	int video_port; 

	int audio_port;

	int cmd_port;

	bool conflict;

	bool network_status;

	unsigned char uuid[8];

	int uuid_length;
}DEVICE_INFO;

typedef struct
{
    DEVICE_BUSY_TYPE dev_online_busy;
	char *ip;
	unsigned long long time;
	char *uuid;
}DEVICE_INFO_STRUCT;

typedef struct
{
	DEVICE_INFO_STRUCT info[DEVICE_TOTAL];
}DEVICE_IP_LIST;

typedef struct{
    DEVICE_IP_LIST DEVICE_LIST;
	network_device tuya_device;
	bool indoor_conflict;
	bool outdoor_conflict;
	bool device_status_update;
}DEV_IP_LIST;


typedef enum
{
	OUTDOOR_CALL_VOL = 0,
	OUTDOOR_TALK_VOL,
	OUTDOOR_VERSION,
	OUTDOOR_PARAM_TOTAL
}OUTDOOR_PARAM;

typedef struct{
    unsigned int OUTDOOR[2][OUTDOOR_PARAM_TOTAL];
}OUTDOOR_GROUP;

#define NET_COMMON_CMD_OUTDOOR_CALL  0X56
#define NET_COMMON_CMD_OUTDOOR_TALK  0X57
#define NET_COMMON_CMD_OUTDOOR_BUYS  0X58
#define NET_COMMON_CMD_OUTDOOR_LEISURE  0X59
#define NET_COMMON_CMD_OUTDOOR_BUYS_ACK  0X5A
#define NET_COMMON_CMD_OUTDOOR_LEISURE_ACK  0X5B
#define NET_COMMON_CMD_DETECTION_ID 0X71
#define NET_COMMON_CMD_DETECTION_ACK 0X72
#define NET_COMMON_CMD_DETECTION_CONFLICT 0X73
#define NET_COMMON_CMD_INTERNAL_CALL  0X60
#define NET_COMMON_CMD_INTERNAL_RING 0x61
#define NET_COMMON_CMD_INTERNAL_BUSY  0X62
#define NET_COMMON_CMD_INTERNAL_CANCEL 0X63
#define NET_COMMON_CMD_INTERNAL_ACK 0X64
#define NET_CMD_UNLOCK_REQUEST       0x10
#define NET_CMD_UNLOCK_RESPONSE      0x11
#define NET_CMD_UNLOCK_TIME_SYNC     0x12
#define NET_CMD_CCTV_SYN_REQUEST      0x23
#define NET_CMD_CCTV_SYN_ACK      0x24
#define NET_CMD_ALARM_FREE_ACK      0x30
#define NET_CMD_ABSENT_POWER      0x3A
#define NET_CMD_ALARM_SYNC_ACK      0x3B
#define NET_CMD_ALARM_SYNC_PASSWORD      0x3C
#define NET_CMD_LOCAL_TIME_SYN_REQUEST      0x31
#define NET_CMD_LOCAL_TIME_SYN_ACK      0x32
#define NET_CMD_OPEN_CAMERA_REQUEST      0x33
#define NET_CMD_END_CAMERA_ACK      0x34
#define NET_CMD_ANSWER_CAMERA_REQUEST       0x35
#define NET_CMD_ANSWER_CAMERA_ACK      0x36

#define NET_CMD_DEVICE_BUSY  0xFD

#define NET_CMD_SET_CAMERA_VOL       0x40

#define NET_CMD_APP_UPGRADE     0x99

#define NET_CMD_TUYA_REGISTER     0xA0

#define NET_CMD_DEVICE_ACK_NONE     0xFE
#define NET_CMD_DEVICE_NONE     0xFF
void device_info_display(DEVICE_INFO device_info);
int get_netlink_status(const char *if_name);
char* get_device_ip(void);
bool  get_locat_ip(char (*ip_addr)[64],char *eth);
void net_common_send(network_device device,char cmd,unsigned char arg1,unsigned char arg2);
void net_common_data_send(network_device device,unsigned char cmd,unsigned char *data,unsigned int size);
void send_lookup_device_cmd(unsigned int arg1,unsigned int arg2,unsigned int update);
bool rtps_video_stream_open(char *url,int width,int height);
bool rtsp_video_stream_close(void);
int rtsp_stream_status_get(void);
bool video_stream_open(int phy_id);
bool video_stream_close();
bool video_stream_switch(int phy_id);
bool video_display_mode_set(char mode);
char vidoe_display_mode_get(void);
void internal_udp_open(const char *ip, int port,NET_SOCK_TYPE net_sock,int vol,int gain);
void internal_udp_close(void);
bool internal_udp_status_get(void);

bool tuya_audio_stream_open(int phy_id);
bool tuya_audio_stream_close(void);

bool audio_push_to_tuya_open(void);
bool audio_push_to_tuya_close(void);

bool audio_pull_to_local_open(void);
bool audio_pull_to_local_close(void);

bool audio_stream_open(int phy_id,int vol,int gain);
bool audio_stream_close(void);
bool audio_stream_status_get(void);


bool video_decode_close(void);

bool network_commom_device_init(network_device self);

network_device tuya_open_device_get(void);

void tuya_open_device_set(network_device dev);

void device_list_ip_set(int device_id,char *ip);

char* device_list_ip_get(int device_id);

bool device_status_update(void);

DEVICE_BUSY_TYPE device_busy_status_get(network_device dev);

bool device_conflict_get(DEVICE_STATUS_TYPE type);

bool get_source_inline_status(int channel);

bool network_connection_status(void);

unsigned int outdoor_info_get(unsigned int outdoor_id,OUTDOOR_PARAM type);

 network_device network_oneself_device_set(network_device device);

void unlock_send_cmd(network_device destination,unsigned char delay ,network_device source);
#endif


