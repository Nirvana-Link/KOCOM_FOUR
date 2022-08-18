#ifndef QUEUE_API_H
#define QUEUE_API_H
#include <stdbool.h>

typedef enum
{
	QUEUE_CHANNEL_VIDEO_1,
	QUEUE_CHANNEL_VIDEO_2,
	QUEUE_CHANNEL_AUDIO_1,
	QUEUE_CHANNEL_4,
	AUDIO_CHANNEL_TOTAL
}QUEUE_CHANNEL;


typedef struct 
{
	unsigned char* addres;
	int size;
	unsigned long long ts;
	bool is_idr_frame;
}queue_data;

bool queue_data_init(QUEUE_CHANNEL ch);

bool queue_data_push(QUEUE_CHANNEL ch,unsigned char* addres,int len,unsigned long ts,bool is_idr);

queue_data* queue_data_pop(QUEUE_CHANNEL ch);

bool queue_data_release(QUEUE_CHANNEL ch);

#endif

