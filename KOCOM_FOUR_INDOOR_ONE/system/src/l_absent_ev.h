#ifndef L_ABSENT_H
#define L_ABSENT_H

#include"l_absent_queue.h"


#define ABSENT_EV_FILE_PATH     "/etc/config/event.txt"

void absent_event_init(absent_ev_array *event_group);
void absent_event_save(char *param_file,absent_ev_array *event_group);
void absent_event_read(char *param_file,absent_ev_array *event_group);

#endif