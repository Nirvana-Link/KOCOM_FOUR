#include "layout_define.h"
#include "string.h"
#include "leo_tuya_key_check.h"
#include "tuya_sdk.h"


static button tuya_key_apply_btn;
/**********************************
存储input password 的数字
*********************************/
static char tuya_key_pwd_buffer_temp[4];
static char tuya_key_input_pwd_buffer[4];
static int tuya_key_input_num_index = 0;
static int tuya_key_input_temp_index = 0;
static button tuya_key_input_pwd_btn_group[11];


static char is_tuya_key_input_again = 0;

static void tuya_key_input_serial_number_text_display(void) {
    position pos = {{500, 191},
                    {400, 60}};
    text text;
    text_init(&text, &pos, 30);
    text.align = CENTER_MIDDLE;
    text.erase = 1;
    if (is_tuya_key_input_again == 0) {
        char *str = {"Input enter serial number"};
        text_display(&text, str);
    } else {
        char *str = {"Input enter serial number again"};
        text_display(&text, str);
    }
}

static void tuya_key_input_serial_number_line_display(void) {

    position pos = {{632, 335},
                    {150, 3}};
    gui_erase(&pos, 0xFFFFFF);
}


static void tuya_key_pwd_number_display(void) {

    text text;
    position pos = {{632, 254},
                    {150, 80}};
    text_init(&text, &pos, 45);
    text.erase = 1;

    char buffer[10] = {0};
    if (tuya_key_input_num_index == 1) {
        sprintf(buffer, "%d", tuya_key_input_pwd_buffer[0]);
    } else if (tuya_key_input_num_index == 2) {
        sprintf(buffer, "%d%d", tuya_key_input_pwd_buffer[0], tuya_key_input_pwd_buffer[1]);
    } else if (tuya_key_input_num_index == 3) {
        sprintf(buffer, "%d%d%d", tuya_key_input_pwd_buffer[0], tuya_key_input_pwd_buffer[1],
                tuya_key_input_pwd_buffer[2]);
    } else if (tuya_key_input_num_index == 4) {
        sprintf(buffer, "%d%d%d%d", tuya_key_input_pwd_buffer[0], tuya_key_input_pwd_buffer[1],
                tuya_key_input_pwd_buffer[2],
                tuya_key_input_pwd_buffer[3]);
    } else {
        text_display(&text, NULL);
        return;
    }
    text_display(&text, buffer);
}


static void tuya_key_apply_btn_display(void) {

    button_display(&tuya_key_apply_btn);
    if (tuya_key_input_num_index != 0) {
        button_enable(&tuya_key_apply_btn);
    } else {
        button_disable(&tuya_key_apply_btn);
    }
}

static void tuya_key_apply_btn_up(const void *arg) {

    if (is_tuya_key_input_again == 0) {

        tuya_key_input_temp_index = tuya_key_input_num_index;
        strncpy(tuya_key_pwd_buffer_temp, tuya_key_input_pwd_buffer, tuya_key_input_temp_index);

        tuya_key_input_num_index = 0;
        memset(tuya_key_input_pwd_buffer, 0, sizeof(tuya_key_input_pwd_buffer));

        is_tuya_key_input_again = 1;
        tuya_key_input_serial_number_text_display();
        tuya_key_pwd_number_display();
        tuya_key_apply_btn_display();

    } else {

        if ((tuya_key_input_temp_index != tuya_key_input_num_index) ||
            (strncmp(tuya_key_pwd_buffer_temp, tuya_key_input_pwd_buffer, tuya_key_input_num_index) != 0)) {
            is_tuya_key_input_again = 0;
            tuya_key_input_temp_index = tuya_key_input_num_index = 0;
            memset(tuya_key_input_pwd_buffer, 0, sizeof(tuya_key_input_pwd_buffer));
            tuya_key_input_serial_number_text_display();
            tuya_key_pwd_number_display();
            tuya_key_apply_btn_display();
        } else {
            /**********************
            验证成功
            **********************/
            int serial = 0;
            if (tuya_key_input_num_index == 1) {
                serial = tuya_key_input_pwd_buffer[0];
            } else if (tuya_key_input_num_index == 2) {
                serial = tuya_key_input_pwd_buffer[0] * 10 + tuya_key_input_pwd_buffer[1];
            } else if (tuya_key_input_num_index == 3) {
                serial = tuya_key_input_pwd_buffer[0] * 100 + tuya_key_input_pwd_buffer[1] * 10 +
                         tuya_key_input_pwd_buffer[2];
            } else if (tuya_key_input_num_index == 4) {
                serial = tuya_key_input_pwd_buffer[0] * 1000 + tuya_key_input_pwd_buffer[1] * 100 +
                         tuya_key_input_pwd_buffer[2] * 10 +
                         tuya_key_input_pwd_buffer[3];
            } else {
                return;
            }

            if (tuya_product_seral_write(serial) == true) {
                os_layout_goto(&layout_home);
            } else {
                is_tuya_key_input_again = 0;
                tuya_key_input_temp_index = tuya_key_input_num_index = 0;
                memset(tuya_key_input_pwd_buffer, 0, sizeof(tuya_key_input_pwd_buffer));
                tuya_key_input_serial_number_text_display();
                tuya_key_pwd_number_display();
                tuya_key_apply_btn_display();
            }
        }
    }
}

static void tuya_key_apply_btn_init(void) {
    position pos = {{420, 508},
                    {185, 60}};
    button_init(&tuya_key_apply_btn, &pos, tuya_key_apply_btn_up);
    resource res = resource_get(ROM_R_0_IMG_SURVIVAL_TIME_BG_PNG);
    button_icon_init(&tuya_key_apply_btn, &res);

    static char *str[language_total] = {
            "apply",
            "apply"
    };
    button_text_init(&tuya_key_apply_btn, &str, 24);
    tuya_key_apply_btn.erase = 1;
}

static void tuya_key_input_pwd_btn_group_display(void) {
    char *str_text[12] = {
            "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "X"
    };

    text text;
    for (int i = 0; i < 11; i++) {
        button_display(&tuya_key_input_pwd_btn_group[i]);
        text_init(&text, &tuya_key_input_pwd_btn_group[i].pos, 32);
        text_display(&text, str_text[i]);
    }
}

static void tuya_key_input_pwd_btn_group_up(const void *arg) {
    button *p_btn = (button *) arg;

    for (int i = 0; i < 11; i++) {
        if (p_btn == (&tuya_key_input_pwd_btn_group[i])) {
            if (i != 10) {
                /**************
                输入密码
                ***************/
                if (tuya_key_input_num_index > 3) {
                    return;
                }
                tuya_key_input_pwd_buffer[tuya_key_input_num_index++] = ((i + 1) % 10);
                if (tuya_key_input_num_index == 1) {
                    tuya_key_apply_btn_display();
                }
                tuya_key_pwd_number_display();
            } else {
                /**************
                删除密码
                ***************/
                if (tuya_key_input_num_index < 1) {
                    return;
                }
                tuya_key_input_num_index--;
                /***************************************
                如果已经最后一次输入密码，且已经是第4位
                则取消监听此按钮
                ****************************************/
                if (tuya_key_input_num_index == 0) {
                    tuya_key_apply_btn_display();
                }
                tuya_key_pwd_number_display();
            }
            break;
        }
    }

}

static void tuya_key_input_pwd_btn_group_init(void) {
    position pos_group[11] = {
            {{141, 96},  {92, 92}},
            {{255, 96},  {92, 92}},
            {{369, 96},  {92, 92}},
            {{141, 196}, {92, 92}},
            {{255, 196}, {92, 92}},
            {{369, 196}, {92, 92}},
            {{141, 296}, {92, 92}},
            {{255, 296}, {92, 92}},
            {{369, 296}, {92, 92}},
            {{255, 396}, {92, 92}},
            {{369, 396}, {92, 92}}
    };
    resource res = resource_get(ROM_R_0_IMG_RINPUT_BLACK_PNG);
    for (int i = 0; i < 11; i++) {

        button_init(&tuya_key_input_pwd_btn_group[i], &pos_group[i], tuya_key_input_pwd_btn_group_up);
        button_icon_init(&tuya_key_input_pwd_btn_group[i], &res);
    }
}


static void tuya_key_enter(void) {

    is_tuya_key_input_again = 0;
    tuya_key_input_num_index = 0;
    tuya_key_input_temp_index = 0;
    memset(tuya_key_pwd_buffer_temp, 0, sizeof(tuya_key_pwd_buffer_temp));
    memset(tuya_key_input_pwd_buffer, 0, sizeof(tuya_key_input_pwd_buffer));

    tuya_key_apply_btn_init();
    tuya_key_input_pwd_btn_group_init();

    tuya_key_input_pwd_btn_group_display();
    tuya_key_apply_btn_display();
    tuya_key_input_serial_number_text_display();
    tuya_key_input_serial_number_line_display();

    for (int i = 0; i < 11; i++) {
        button_enable(&tuya_key_input_pwd_btn_group[i]);
    }
}

static void tuya_key_quit(void) {
    // if(user_data_get()->wlan.status){
    //     turn_on_wlan_connect();
    // }else{
    //     turn_off_wlan_connect();
    // }
    // tuya_dp_232_response_absent_mode(user_data_get()->security.running);
    //  tuya_wifi_sdk_init(IPC_APP_PID,"tuya48603d9f736f3164","AQGxkOaXZp7G4MMJxFrfLTRPxSfrcEXz");
}

static void tuya_key_timer(void) {
}

layout layout_tuya_key = {
        .enter = tuya_key_enter,
        .quit = tuya_key_quit,
        .timer = tuya_key_timer,
};

