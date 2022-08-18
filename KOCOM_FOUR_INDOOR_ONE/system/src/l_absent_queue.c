#include "l_absent_queue.h"
#include "ak_mem.h"
#include <memory.h>
#include "ak_thread.h"
#include"debug.h"


#define EVENT_QUEUE

#ifdef EVENT_QUEUE
void absent_event_queue_init( queue *Link){
    Link->head = Link->tail = ( queue_ptr)malloc(sizeof( queue_node));
    Link->index = 0;
    if(Link->head == NULL){
        exit (0);
    }
    Link->head->next = NULL;
    Link->head->prev = NULL;
}

bool Is_event_queue_empty( queue Link){
    return (Link.head == Link.tail )? true:false;
}
/* 入队 */
void absent_event_enqueue( queue *Link,char *event){
     queue_ptr newNode = ( queue_ptr)malloc(sizeof( queue_node));
    if(newNode == NULL){
        exit (0);
    }
    sprintf(newNode->event,"%s",event);
    newNode->pitch_ev = false;
    newNode->next = NULL;
    newNode->prev = Link->tail;

    Link->tail->next = newNode;
    Link->tail = newNode;
    Link->index ++;
}
/* 出队 */
void absent_event_dequeue( queue *Link){
     queue_ptr q = NULL;
    if(Is_event_queue_empty(*Link) == false){
        q = Link->head->next;
        q->prev = Link->head;
        Link->head->next = q->next;
        Link->index --;
        if(Link->tail == q){
            Link->tail = Link->head;
        }
        free(q);
    }
}

void absent_event_traversal_queue( queue Link){
    int i = 1;
     queue_ptr q = Link.head->next;
    while (q != NULL){
        // DEBUG_LOG("DEV :%d      MSG : %x\n\r",q->dev+1,q->msg);
        DEBUG_LOG("pitch : %d     event :%s  \n\r",q->pitch_ev,q->event);
        q = q->next;
        i++;
    }
}

void absent_event_empty_queue(queue *Link){
    queue_ptr p,q;
	Link->tail = Link->head;//跟初始状态相同，Link->tail指向头结点
	p = Link->head->next;//开始销毁队头元素，队头，对尾依然保留
	Link->head->next = NULL;
    Link->head->prev = NULL;
    Link->index = 0;
	while(p != NULL){
		q = p;
		p = p->next;
		free(q);
	}	
}
#endif

#ifdef EVENT_ARRAY
void absent_event_array_init(absent_ev_array *array){
    array->size = 0;
    memset(array->channel,0,sizeof(array->channel));
    memset(array->absent_ev_group,0,sizeof(array->absent_ev_group));
}

void absent_event_array_upadte(absent_ev_array *array,char *data,unsigned char channel_str_id,unsigned char device_str_id){
    int array_index =( array->size == 20 ? 19 : array->size);

    if(array_index == 0){
        array->channel[array->size] = channel_str_id;
        array->device[array->size] = device_str_id;
        memcpy(array->absent_ev_group[array->size++],data,sizeof(array->absent_ev_group[0]));
        return;
    }
    while(array_index){
        array->channel[array_index] = array->channel[array_index-1];
        array->device[array_index] = array->device[array_index-1];
        memcpy(array->absent_ev_group[array_index],array->absent_ev_group[array_index-1],sizeof(array->absent_ev_group[0]));
        array_index--;
    }

    array->channel[array_index] = channel_str_id;
    array->device[array_index] = device_str_id;
    memcpy(array->absent_ev_group[array_index],data,sizeof(array->absent_ev_group[0]));
    
    array->size = array->size < 20 ? array->size+1 : 20;

    // DEBUG_LOG(" ADD ABSENT EVENT :%d     %s\n\r\n\r",array->channel[0],array->absent_ev_group[0]);
}

void absent_event_array_traversal(absent_ev_array *array){

    for(int i = 0;i < array->size;i++){
        DEBUG_LOG(" ABSENT EVENT [%d] : %s\n\r\n\r",i,array->absent_ev_group[i]);
    }
}

#endif