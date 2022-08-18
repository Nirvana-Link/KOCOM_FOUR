#ifndef LAYOUT_CAPTURE_BUTTON_H
#define LAYOUT_CAPTURE_BUTTON_H
#include"ui_api.h"
#include"l_user_config.h"
enum capt_common_but{
    CLEAR_ALL_BUTTON,
    CLEAR_BUTTON,
    LEFT_BUTTON,
    RIGHT_BUTTON,
    FLASH_BUTTON,
    SD_BUTTON,
    PUASE_BUTTON,
    ACK_BUTTON,
    CANCEL_BUTTON,
    CAPT_BUTTON_TOTAL
};
enum capt_file_button{
    FILE_1_BUTTON,
    FILE_2_BUTTON,
    FILE_3_BUTTON,
    FILE_4_BUTTON,
    FILE_5_BUTTON,
    FILE_6_BUTTON,
    SCREE_RETURN_BUTTON,
    FILE_BUTTON_TOTAL
};

typedef struct capt_file_but
{
    button file_but;
    bool pitch_flag;
}CAPT_FILE;
button CAPT_COMM_BUT[CAPT_BUTTON_TOTAL];
CAPT_FILE CAPT_FILE_BUT[FILE_BUTTON_TOTAL];
#endif