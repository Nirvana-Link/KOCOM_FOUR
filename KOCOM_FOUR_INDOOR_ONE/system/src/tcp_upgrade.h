//
// Created by michael on 2022/1/21.
//

#ifndef KCV_T701BT_TCP_UPGRADE_H
#define KCV_T701BT_TCP_UPGRADE_H
#include "stdbool.h"

// TUF: TCP UPGRADE FILE
typedef enum upgrade_status{
    TUF_EMPTY,
    TUF_EXISTS,
    TUF_DAMAGED,
    TUF_ROLL_BACK,
    TUF_INVALID_VERSION,
    TUF_UPDATE,
    TUF_TRANSFER,
    TUF_TRANSFER_FAIL,
    TUF_TRANSFER_DAMAGED,
    TUF_FINISH,
    TUF_TOTAL
}tcp_status;

bool server_update_file_exist(void);
tcp_status upgrade_server_status(void);

bool create_upgrade_server_task(void);
bool destroy_upgrade_server_task(void);
int connect_server_upgrade(char *upgrade_server_ip);

bool create_upgrade_client_task(char *server_ip);
bool destroy_upgrade_client_task(void);

bool upgrade_outdoor_status(void);
void set_upgrade_outdoor_status(bool status);

#endif //KCV_T701BT_TCP_UPGRADE_H
