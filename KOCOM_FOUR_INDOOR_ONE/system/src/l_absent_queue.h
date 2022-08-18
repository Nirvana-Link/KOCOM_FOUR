#ifndef L_QUEUE_H
#define L_QUEUE_H

#include <stdbool.h>

#define EVENT_ARRAY

typedef struct  queue_node{
    char event[64];
    bool pitch_ev;
    struct  queue_node *prev;
    struct  queue_node *next;
} queue_node,* queue_ptr;

typedef struct  queue{
     queue_ptr head;
     queue_ptr tail;
    int index;
} queue;

queue absent_event;
void absent_event_queue_init( queue *Link);
bool Is_event_queue_empty( queue Link);
void absent_event_enqueue( queue *Link,char *event);
void absent_event_dequeue( queue *Link);
void absent_event_traversal_queue( queue Link);
void absent_event_empty_queue( queue *Link);


typedef struct  abs_array{
    unsigned char absent_ev_group[20][32];
    unsigned char channel[20];
    unsigned char device[20];
    int size;
} absent_ev_array;

absent_ev_array absent_event_group;

void absent_event_array_init(absent_ev_array *array);
void absent_event_array_upadte(absent_ev_array *array,char *data,unsigned char channel_str_id,unsigned char device_str_id);
void absent_event_array_traversal(absent_ev_array *array);
#endif
