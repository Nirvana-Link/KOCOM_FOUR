#ifndef LEO_FILE_LIST_H
#define LEO_FILE_LIST_H
#include "stdbool.h"

#define MEDIA_PATH_MAX 128

#define SD_MIXED_MAX 1000
#define SD_PHOTO_MAX 500
#define SD_VIDEO_MAX 500
#define SD_AUDIO_MAX 20
#define FLASH_PHOTO_MAX 50
#define SD_GALLERY_MAX 30

#define FILE_NAME_MAX 23


#define SD_BASE_PATH "/mnt/tf"

#define SD_MIXED_PATH  SD_BASE_PATH"/media/"
#define SD_PHOTO_PATH  SD_BASE_PATH"/photo/"
#define SD_VIDEO_PATH  SD_BASE_PATH"/video/"
#define SD_AUDIO_PATH  SD_BASE_PATH"/message/"
#define SD_GALLERY_PATH SD_BASE_PATH"/gallery/"
#define	FLASH_PHOTO_PATH "/etc/config/photo/"
#define SD_BACKUP_PATH  SD_BASE_PATH"/backup/"

#define MIX_PHOTOS_AND_VIDEOS_FILE (1)
#define AUDIO_MESSAGE_FILE_ENABLE  (0)
#define GALLERY_IMG_FILE_ENABLE    (0)

typedef enum{

	RECORD_MODE_MANUAL = 0,
	RECORD_MODE_AUTO
}record_mode;

typedef enum
{
    FILE_TYPE_SD_MIXED,
    FILE_TYPE_SD_MIXED_PHOTO,
    FILE_TYPE_SD_MIXED_VIDEO,
    FILE_TYPE_SD_PHOTO,
    FILE_TYPE_SD_VIDEO,
    FILE_TYPE_SD_AUDIO,
    FILE_TYPE_SD_GALLERY,
    FILE_TYPE_FLASH_PHOTO,
    FILE_TYPE_NONE
}media_type;

enum delete_flag{
    DELETE_FINISH_STOP = 0X00,
    DELETE_ALL_MIXED =0X01,
    DELETE_ALL_SD_PHOTO =0X02,
    DELETE_ALL_SD_VIDEO =0X04,
    DELETE_ALL_MIXED_PHOTO = 0X08,
    DELETE_ALL_MIXED_VIDEO = 0X10,
    DELETE_ALL_FLASH_PHOTO =0X20,
    DELETE_ALL_MESSAGE =0X40,
    DELETE_ALL_GALLERY =0X80
};

typedef struct 
{
    char file_name[FILE_NAME_MAX];
    char mode;
    char ch;
    char is_new;
    media_type type;
}media_info;

void media_file_list_init(void);
bool is_sdcard_insert(void);
bool create_one_media_file(media_type type,char ch,char mode,char* file);
media_info* media_info_get(media_type type,int index);
int media_file_total_get(media_type type,char is_new);
int media_file_new_clear(media_type type,int index);
int media_file_delete(media_type type,int index);
bool media_bad_path_check(const char* file);
bool Media_bad_path_check(const char* file,int index);

void start_format_sd_card(void);
/* @return: true: formatting false:finish or stop*/
bool format_sd_card_status(void);

void start_copy_flash_photo_to_sd(void);
/* @return: true: copying false:finish or stop*/
bool copy_flash_photo_to_sd_status(void);

void start_delete_media(enum delete_flag item);
/* @return: >0: delete false:finish or stop*/
char delete_media_status(void);
#endif


