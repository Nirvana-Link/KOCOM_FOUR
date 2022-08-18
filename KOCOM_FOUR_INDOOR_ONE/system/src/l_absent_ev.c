#include"l_absent_ev.h"
#include"stdio.h"
#include"stdlib.h"
#include"unistd.h"
#include<unistd.h>
#include"string.h"
#include"debug.h"

#define EVENT_ERROR (printf("\n[EVENT FILE] line:%d ",__LINE__),printf)
#define EV_ERROR (printf("\n[absent ev] line:%d ",__LINE__),printf)

#ifdef EVENT_QUEUE
void ev_fread(){
    FILE *fp;
    
    if((fp = fopen(ABSENT_EV_FILE_PATH,"r")) == NULL)
    {
       EV_ERROR("open %s fail.\n\r",ABSENT_EV_FILE_PATH);
       return;
    }
    queue_ptr q = (queue_ptr)malloc(sizeof(queue_node));
    while(1){
        if(fgets(q->event,1024,fp) != NULL){
            char ev[64];
            memset(ev,0,sizeof(ev));
            memcpy(ev,q->event,strlen(q->event) - 1);
            DEBUG_LOG("ev:%s    size:%d\n\r",ev,strlen(ev));
            absent_event_enqueue(&absent_event,ev);
        }else{
            free(q);
            break;
        }
    }
    fclose(fp);
}
void ev_write(queue *event){
    if(Is_event_queue_empty(*event) == true) return;
    FILE *fp;
    queue_ptr ev = event->head->next;
    if((fp = fopen(ABSENT_EV_FILE_PATH,"w")) == NULL)
    {
       EV_ERROR("open %s fail.\n\r",ABSENT_EV_FILE_PATH);
       exit(0);
    }
    while(ev){
        fprintf(fp,"%s\n",ev->event);
        DEBUG_LOG("ev_write------------->>>%s \n\r",ev->event);
        ev = ev->next ;
    }
    fclose(fp);
}
void absent_event_init(void){
    
    absent_event_queue_init(&absent_event);

    // FILE *fp = NULL;
    if(access(ABSENT_EV_FILE_PATH,F_OK) != 0){
        DEBUG_LOG("--------------->>>ABSENT EVENT INEXISTENCE \n\r");
        // fp = fopen(ABSENT_EV_FILE_PATH,"w");
        // if(fp == NULL){
        //     EV_ERROR("open %s fail.\n\r",ABSENT_EV_FILE_PATH);
        //     exit(1);
        // }else{
        //     fclose(fp);
        // }
        ev_write(&absent_event);
    }else{
        // fp = fopen(ABSENT_EV_FILE_PATH,"r");
        // if(fp ==NULL){
        //     EV_ERROR("open %s fail.\n\r",ABSENT_EV_FILE_PATH);
        //     fclose(fp);
        // }
        ev_fread();
    }
}
#endif

#ifdef EVENT_ARRAY

void absent_event_read(char *param_file,absent_ev_array *event_group)
{
    FILE *fp = NULL;
    
    fp = fopen(ABSENT_EV_FILE_PATH,"r");
    if(fp ==NULL)
    {
        EVENT_ERROR("read open %s fail.\n\r",ABSENT_EV_FILE_PATH);
        fclose(fp);
        return;
    }
    fread(event_group,sizeof(*event_group),1,fp);
    fclose(fp);
    // DEBUG_LOG("AAAAAAAAAAAAAAAAAAaevent_group:%s    size:%d\n\r",event_group->absent_ev_group[0],event_group->size);
    // absent_event_array_traversal(event_group);
    return;
}

void absent_event_save(char *param_file,absent_ev_array *event_group)
{
    FILE *fp = NULL;
    fp = fopen(param_file,"w+");    
    if(fp ==NULL)
    {
        EVENT_ERROR("write open %s fail.\n\r",param_file);
        fclose(fp);
        return;
    }
    fwrite(event_group,sizeof(*event_group),1,fp);
    // DEBUG_LOG("BBBBBBBBBBBBBBBBBBBaevent_group:%s    size:%d\n\r",event_group->absent_ev_group[0],event_group->size);
    // fread(&config,sizeof(config),1,fp);
    // DEBUG_LOG("config.user_default_device = %d\n\r",config.user_default_device);
    fclose(fp);
    return;
}

void absent_event_init(absent_ev_array *event_group){
    
    absent_event_array_init(event_group);

    // FILE *fp = NULL;
    if(access(ABSENT_EV_FILE_PATH,F_OK) != 0){
        DEBUG_LOG("--------------->>>ABSENT_EV_FILE_PATH INEXISTENCE \n\r");
        absent_event_save(ABSENT_EV_FILE_PATH,event_group);    
    }else{
        DEBUG_LOG("--------------->>>ABSENT_EV_FILE_PATH EXIST \n\r");
        absent_event_read(ABSENT_EV_FILE_PATH,event_group);   
    }
}

#endif