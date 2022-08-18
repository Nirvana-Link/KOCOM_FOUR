#include"ui_api.h"
#include"os_sys_api.h"
#include"rom.h"
#include"layout_common.h"
#include"math.h"
#include"malloc.h"
#include"layout_base.h"
#include"layout_capture_button.h"
#include"thumb_display_api.h"
#include"video_play_api.h"
#include"debug.h"

#define curr_file_count         ((mixed_curr_page-1)*6) //当前页数之前的所有文件个数
#define curr_file_id(x)                (mixed_file_count - (x) - 1)
static int mixed_file_count = 0;//检测到的文件数
static int mixed_curr_page = 1;//当前显示的页数
static int mixed_curr_index = 0;
static int mixed_tatol_page = 0;//总页数
static bool *select_file = NULL;//文件选中索引 
#if SD_CARD
media_type current_preview_file_type = FILE_TYPE_SD_MIXED;//当前文件的存储类型
#else
media_type current_preview_file_type = FILE_TYPE_FLASH_PHOTO;//当前文件的存储类型
#endif
static int curr_full_screen = FILE_BUTTON_TOTAL;//缩略显示,FILE_BUTTON_TOTAL代表非全屏，其余值代表当前页面第几个文件
static bool is_play_video = false;//是否播放视频


/* 暂停UI-显示 */
void puase_icon_display(position pos)
{
    icon puase;
    resource res = resource_get(ROM_R_6_CAPTURE_IS_VIDEO_PNG);
    icon_init(&puase,&pos,&res);
    icon_display(&puase);
}
/* 文件按键-多选操作 
    but：按键数组
    start：操作的起始下标
    end：终止下标
    *control:：需要执行的操作*/
void file_but_cont_group(CAPT_FILE *but,int start,int end,bool (*control)(const button*))
{
    for(int i = start;i < end ;i++)
    {
        control(&but[i].file_but);
    }
}
/* 查找当前页文件按键索引
    but：按键数组
    Button：要查找的按键
    total：查找范围*/
int SearchFile(CAPT_FILE *but, button *Button , int total)
{
    int i;
    for(i=0; i<total; i++)
        if(Button== &but[i].file_but )
            return i;
    return -1;
}

/* 获得当前文件序号 */
static unsigned get_file_serial(){
    return (mixed_curr_index +1);
}
/* 当前文件序号/总文件数-显示 */
static void curr_serial_display(void)
{
    text text;    
    position pos = {{440,540},{150,38}};
    char serial_dis[16];

    sprintf(serial_dis,"%02d / %02d",get_file_serial(),mixed_file_count);
    gui_erase(&pos,0x00);
    text_init(&text,&pos,font_size(CAPTURE_INFO));
    text.bg_color = TRANSLUCENCE;
    text_display(&text,serial_dis);
}

/* 获得文件数、页数 */
static void get_file_page(void)
{
    if(select_file != NULL)
    {
        free(select_file);
        select_file = NULL;
    }
    mixed_file_count = media_file_total_get(current_preview_file_type,false);
    mixed_tatol_page = mixed_file_count ? mixed_file_count/SCREE_RETURN_BUTTON : 1;
    mixed_tatol_page =  (mixed_file_count%SCREE_RETURN_BUTTON)?mixed_tatol_page+1:mixed_tatol_page;  
    if(mixed_tatol_page == 0)
      mixed_tatol_page = 1;
    if(mixed_curr_page > mixed_tatol_page)
        mixed_curr_page = mixed_tatol_page;
    DEBUG_LOG("mixed_file_count-------------->>>%d \n\r",mixed_file_count);    
    select_file = (bool *)malloc(sizeof(bool)*mixed_file_count);    
    memset(select_file,0,sizeof(bool)*mixed_file_count);
}
/* 获得文件时间
    name：文件名
    data：文件名存储 */
static void get_file_time(char *name,char *data)
{
    char year[5];
    char month[4];
    char day[4];
    char hour[4];
    char minute[4];
    char secode[4];
    sprintf(data, "%s-%s-%s %s:%s:%s", mid(year,name,4,0), mid(month,name,2,4), mid(day,name,2,6), mid(hour,name,2,9), mid(minute,name,2,11),mid(secode,name,2,13) );   
}
/* 文件时间-显示
    文件时间
    x：文件时间显示坐标x
    y：文件按键坐标y
    color：背景颜色
    font_size：显示大小 */
static void media_item_time_display(char *name,int x,int y,int width,unsigned int color,int font_size)
{
    text info;
    position pos = {{45,284},{300,42}};
    pos.point.x = x;
    pos.point.y = y+138;//显示高度在按键区域的下方，因此需要加138
    pos.vector.width = width;
    text_init(&info,&pos,font_size);
    info.align = RIGHT_MIDDLE;
    info.offset.x = -10;
    info.bg_color = color;
    info.erase = true;
    char data[32];
    get_file_time(name,data);
    DEBUG_LOG("name :%s=====================>data%s\n\r",name,data);
    text_display(&info,data);
}


/* 文件路径-显示
    文件路径
    x：文件路径显示坐标x
    y：文件按键坐标y
    color：背景颜色
    font_size：显示大小 */
static void media_item_ch_display(char ch,int x,int y,int width,unsigned int color,int font_size,bool full_screen)
{
    text info;
    position pos = {{45,284},{75,42}};
    pos.point.x = x+10;
    pos.point.y = y+138;//显示高度在按键区域的下方，因此需要加138
    pos.vector.width = width;
    text_init(&info,&pos,font_size);
    info.align = LEFT_MIDDLE;
    info.offset.x = full_screen ? info.offset.x+10:info.offset.x;
    info.bg_color = color;
    info.erase = true;
    // char data[32];
    // get_file_ch(ch,data);
    language_text_display(&info,text_str(STR_SENSOR2+ch),language_get());
}
/* 当前页/总页数-显示 */
static void page_display(void)
{
    text text;    
    position pos = {{465,545},{100,36}};
    char page_dis[16];

    sprintf(page_dis,"%02d / %02d",mixed_curr_page,mixed_tatol_page);
    gui_erase(&pos,0x00);
    text_init(&text,&pos,24);
    text_display(&text,page_dis);
}
/* 文件选中标志-显示
    i：文件按键下标
    on：是否选中 */
static void pitch_on_display(int i,bool on)
{
    gui_erase(&CAPT_FILE_BUT[i].file_but.pos,0x00);
    if(on)
    {
        CAPT_FILE_BUT[i].file_but.resource.id = ROM_R_6_CAPTURE_PITCH_ON_PNG;
        CAPT_FILE_BUT[i].file_but.resource.size = ROM_R_6_CAPTURE_PITCH_ON_PNG_SIZE;
    }
    else
    {
        CAPT_FILE_BUT[i].file_but.resource.id = ROM_R_6_CAPTURE_PITCH_PNG;
        CAPT_FILE_BUT[i].file_but.resource.size = ROM_R_6_CAPTURE_PITCH_PNG_SIZE;        
    }
    CAPT_FILE_BUT[i].file_but.icon_align = LEFT_TOP;
    CAPT_FILE_BUT[i].file_but.icon_offset.x = 258;
    CAPT_FILE_BUT[i].file_but.icon_offset.y = 6;
    button_display(&CAPT_FILE_BUT[i].file_but);    

}
/* 新文件-标志-显示 */
static void new_file_icon_display(position pos){
    icon new;
    resource res = resource_get(ROM_R_6_CAPTURE_IS_NEW_PNG);
    icon_init(&new,&pos,&res);
    new.align = LEFT_TOP;
    icon_display(&new);
}
/* 文件效果-显示
    pos：效果显示区域
    num：当前页面第几个文件
    is_thumb： 是否缩略图显示
    返回值：文件损坏或显示失败返回false*/
static bool file_display(position pos,int num,bool is_thumb)
{
            bool is_bad = false;
            char file_path[64];
            int file_id = curr_file_id(num);//因curr_file_count、num都是从0开始，文件数目比文件数组下标多1，因此要减去1
            // DEBUG_LOG("num =====> %d\n\r",num +1);
            media_info *info = media_info_get(current_preview_file_type,file_id);    
            // DEBUG_LOG("info->type :%d   file_id:%d   \n\r",info->type,file_id); 
    #if SD_CARD
            if(current_preview_file_type == FILE_TYPE_SD_MIXED)
            strcpy(file_path,SD_MIXED_PATH);
            else
    #endif
            strcpy(file_path,FLASH_PHOTO_PATH);

            strcat(file_path,info->file_name);

            if(is_sdcard_insert())
                is_bad = Media_bad_path_check(file_path,file_id);//检测到坏的文件删除掉

            if(is_bad)  return false;            

            if(is_thumb){
                media_item_time_display(info->file_name,pos.point.x,pos.point.y,300,0x80010101,18);
                media_item_ch_display(info->ch,pos.point.x,pos.point.y,100,0x80010101,18,false);
                
                if(info->is_new == true){
                    new_file_icon_display(pos);
                }
            }
            else{
                curr_serial_display();
                media_item_time_display(info->file_name,420,-110,320,0xff23262b,26);   
                media_item_ch_display(info->ch,290,-110,130,0xff23262b,26,true);      
                if(info->is_new == true){
                    media_file_new_clear(current_preview_file_type,file_id);
                }
            }

            // DEBUG_LOG("info->type ---------------------->%d\n",info->type);
            if((info->type != FILE_TYPE_SD_MIXED_VIDEO)){//当前显示类型为照片时
                thumb_data_load(&pos,file_path,false);    
            }
            else  if(curr_full_screen == FILE_BUTTON_TOTAL){//当前显示为视频且非全屏显示
                //DEBUG_LOG("file_path ---------------------->%s\n",file_path);
                network_h264_thumb_data_load(&pos, file_path, true);
                // thumb_data_load(&pos,file_path,true);   
                puase_icon_display(pos);
            }
            else {//视频播放
                video_play_start(file_path,false);
                is_play_video = true;
                button_enable(&CAPT_COMM_BUT[PUASE_BUTTON]);
            }
        return true;
}
/* 文件缩略图-预览窗格
根据序号从大到小排序
以便最近拍摄照片优先显示 */
static void mixed_file_list_display(bool is_get_file_num)
{ 
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    if(is_get_file_num) get_file_page();
    
    // wallpaper_dispaly(IMG_BLACK);//缩略图与背景属于同一层，因此gui_erase对缩略图无效
    set_gui_background_color(MAIN_BG_COLOUR);
    page_display();
    for(int i = 0;i < SCREE_RETURN_BUTTON;i++)
    {
        // DEBUG_LOG("curr_file_count+i------------->>>%d \n\r",curr_file_count+i);        
        if((curr_file_count+i) < mixed_file_count)
        {
            pitch_on_display(i,select_file[curr_file_id(curr_file_count+i)]);           
            // DEBUG_LOG("AAAAAAAAAAAAAAAAAA:%d\n\r",select_file[curr_file_id(curr_file_count+i )]);
            if(file_display(CAPT_FILE_BUT[i].file_but.pos,curr_file_count+i,true) == false){//显示失败，文件可能删除，重新读取文件及个数
                mixed_file_count = media_file_total_get(current_preview_file_type,false);
                i--;//使重新回到上一文件下标，在for一次循环后重新回到当前访问下标
            }
            
            button_enable(&CAPT_FILE_BUT[i].file_but);
        }
        else
        {
            gui_erase(&CAPT_FILE_BUT[i].file_but.pos,0x00);
            button_disable(&CAPT_FILE_BUT[i].file_but);
        }
    }
}
/* 视频播放-进度条-显示
    cur：当前播放时长
    total：视频总时长 */
static void video_play_progress_display(int cur,int total)
{
    text time;
    position bg_bar = {{54,532},{916,6}};
    position pg_bar = {{54,532},{0,6}};
    position time_text = {{880,546},{100,42}};
    cur = cur/10*10;
    total = total/10*10;//去掉十多毫秒误差
    int sec = cur/1000%60;
    int miu =  cur/60000;
    // int sec = (total-cur_time)>200?(total_time-cur_time)/1000+1:(total_time-cur_time)/1000;//时长四舍五入向上取整
    char Sec[6];
    sprintf(Sec,"%02d:%02d",miu,sec);

    // DEBUG_LOG("cur_time------------->>>%d \n\r",cur_time);
    DEBUG_LOG("time :%d:%d\n\r",total,cur);    
   float x = 916/((float)total/100);//进度条每次变动大小


    // float x = (float)((bg_bar.vector.width-pg_bar.vector.width)/((total-cur)/100));
//    x = ((float)((int)(x*1000)))/1000;
   DEBUG_LOG("x------------->>>%f \n\r",x);
    pg_bar.vector.width = (int)round(((float)cur*x/100));//强制转换成浮点型是为了保留毫秒时间，减少误差


    text_init(&time,&time_text,36);
    time.align = LEFT_MIDDLE;
    gui_erase(&time_text,0x00);
    text_display(&time,Sec);
    gui_erase(&bg_bar,0x00);
    draw_rect(&bg_bar,GRAY);
    draw_rect(&pg_bar,BLUE);        
}

static void capture_button_open(void);
#if SD_CARD   
static void memory_status_action(bool is_sd_insert);
#endif
static void file_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int but_id = SearchFile(CAPT_FILE_BUT,(Par_Button),FILE_BUTTON_TOTAL);
    int file_id = curr_file_id(curr_file_count + but_id )/* mixed_file_count - curr_file_count - but_id -1 */;
    const point * cur_point = touch_current_point();
    // DEBUG_LOG("%d - %d : %d    %d - %d :%d\n ",cur_point->x,Par_Button->pos.point.x,cur_point->x - Par_Button->pos.point.x,cur_point->y, Par_Button->pos.point.y,cur_point->y - Par_Button->pos.point.y);
    if(cur_point->x - Par_Button->pos.point.x > 260 && cur_point->y - Par_Button->pos.point.y < 40)
    {
        select_file[file_id] = !select_file[file_id];//改变选中状态
        CAPT_FILE_BUT[but_id].pitch_flag = select_file[file_id];//对应文件按键选中或取消选中       
        DEBUG_LOG("pitch_flag[%d]-------------->>>%d \n\r",file_id,select_file[file_id]);     
        pitch_on_display(but_id,CAPT_FILE_BUT[but_id].pitch_flag);
        file_display(CAPT_FILE_BUT[but_id].file_but.pos,curr_file_count + but_id,true);
    }
    else
    {
        position pos = {{0,0},{1024,600}};
        gui_erase(&pos,0x00);
        set_gui_background_color(MAIN_BG_COLOUR);
        DEBUG_LOG("--------------->>>FILE_%d_BUTTON \n\r",but_id+1);
        switch (but_id)
        {
        case FILE_1_BUTTON:
        case FILE_2_BUTTON:
        case FILE_3_BUTTON:
        case FILE_4_BUTTON:
        case FILE_5_BUTTON:
        case FILE_6_BUTTON:
            CAPT_COMM_BUT[LEFT_BUTTON].bg_color = CAPT_COMM_BUT[RIGHT_BUTTON].bg_color = DEEP_TRANSPARENT;
            // standby_timer_close();
            break;       
        case SCREE_RETURN_BUTTON:
            DEBUG_LOG("--------------->>>SCREE_RETURN_BUTTON \n\r");
            CAPT_COMM_BUT[LEFT_BUTTON].bg_color = CAPT_COMM_BUT[RIGHT_BUTTON].bg_color = LUCE_BLACK;
            curr_full_screen = FILE_BUTTON_TOTAL;
            // standby_timer_open(standby_time,NULL);
            button_refresh(&CAPT_COMM_BUT[LEFT_BUTTON],ROM_R_6_CAPTURE_LEFT_N_PNG,ROM_R_6_CAPTURE_LEFT_N_PNG_SIZE,true);
            button_refresh(&CAPT_COMM_BUT[RIGHT_BUTTON],ROM_R_6_CAPTURE_RIGHT_N_PNG,ROM_R_6_CAPTURE_RIGHT_N_PNG_SIZE,true);
            button_disable(&CAPT_FILE_BUT[SCREE_RETURN_BUTTON].file_but);
            button_disable(&CAPT_COMM_BUT[PUASE_BUTTON]);
            if(is_play_video)  video_play_stop();
            headline_display(false);
            capture_button_open();
            #if SD_CARD   
                memory_status_action(is_sdcard_insert());
            #else
            mixed_file_list_display(true);          
            #endif
            return;       
        default:
            break;
        }
        curr_full_screen = but_id;
        mixed_curr_index = curr_file_count + but_id;
        button_disable(&goto_home_button);
        button_refresh(&CAPT_COMM_BUT[LEFT_BUTTON],ROM_R_6_CAPTURE_LEFT_PNG,ROM_R_6_CAPTURE_LEFT_PNG_SIZE,true);
        button_refresh(&CAPT_COMM_BUT[RIGHT_BUTTON],ROM_R_6_CAPTURE_RIGHT_PNG,ROM_R_6_CAPTURE_RIGHT_PNG_SIZE,true);
        button_disable(&CAPT_COMM_BUT[CLEAR_ALL_BUTTON]);
        button_control_group(CAPT_COMM_BUT,FLASH_BUTTON,PUASE_BUTTON,button_disable);       
        file_but_cont_group(CAPT_FILE_BUT,FILE_1_BUTTON,SCREE_RETURN_BUTTON,button_disable);   
        headline_display(true);   
        button_display(&CAPT_COMM_BUT[CLEAR_BUTTON]);
        button_start_using(&CAPT_FILE_BUT[SCREE_RETURN_BUTTON].file_but);
        file_display(pos,mixed_curr_index,false);
        DEBUG_LOG("The index of the file being viewed : %d\n\r",mixed_curr_index);        
    }
}
/* 文件按键-长按回调 */
static void file_button_long_down(const void *arg)
{
#if 0
    switch (but_id)
    {
    case FILE_1_BUTTON:
        break;
    case FILE_2_BUTTON:
        break;
    case FILE_3_BUTTON:
        break;
    case FILE_4_BUTTON:
        break;
    case FILE_5_BUTTON:
        break;
    case FILE_6_BUTTON:
        break;
       
    default:
        break;
    }
#endif

}
/* 文件按键-初始化接口 */
static void file_button_init(button *BUTTON,position pos)
{
    button_init(BUTTON,&pos,file_button_up);
    BUTTON->long_down = file_button_long_down;
}
/* 文件按键-集体初始化 */
static void file_button_list(void)
{
    position pos = {{48,140},{300,180}};
    for(int file_but = FILE_1_BUTTON;file_but < SCREE_RETURN_BUTTON;file_but++)
    {
        pos.point.y = ((file_but < FILE_4_BUTTON)? 140 : 340);       

        if(0 == file_but%FILE_4_BUTTON)
            pos.point.x = 48;

        file_button_init(&CAPT_FILE_BUT[file_but].file_but,pos);
        pos.point.x += 316;
    }
    position pos_b = {{5,12},{93,89}}; 
    resource res = resource_get(ROM_R_5_INTERNAL_CALL_RETURN_PNG);
    file_button_init(&CAPT_FILE_BUT[SCREE_RETURN_BUTTON].file_but,pos_b);
    button_icon_init(&CAPT_FILE_BUT[SCREE_RETURN_BUTTON].file_but,&res);
    
}

static button *backup_button_group[BUTTON_MAX];//被销毁之前的按键集合
static int backup_button_total = 0;//被销毁的总数
/* 确认删除函数 */
static void clear_ack_oper(int clear_action)
{
    if(video_play_state_get() == VIDEO_PLAY_STOP)        
        confirm_interface(clear_action == CLEAR_ALL_BUTTON ? STR_DELETE_ALL_PHOTO:STR_DELETE_PHOTO,STR_TOTAL,false,0xcc000000,font_size(CAPTURE_INFO) - 4);
    else{
        confirm_interface(STR_DELETE_ALL_PHOTO,STR_TOTAL,false,0xcc000000,font_size(CAPTURE_BUTTON));           
        is_play_video = false; 
        if(video_play_state_get() == VIDEO_PLAY_ING)
        video_play_state_set(VIDEO_PLAY_PAUSE);
    }
    disable_prev_lay_button(backup_button_group,&backup_button_total); //销毁当前所有按键     
    button_start_using(&CAPT_COMM_BUT[ACK_BUTTON]);//使能确认键
    button_start_using(&CAPT_COMM_BUT[CANCEL_BUTTON]);//使能取消键
}
static void switch_picture(void){
    position pos = {{0,0},{1024,600}};

    mixed_curr_page = (mixed_curr_index / 6) + 1;
    DEBUG_LOG("The index of the file being viewed : %d\n\r",mixed_curr_index);
    file_display(pos,mixed_curr_index,false);
}

static void common_button_up(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int but_id = SequenceSearch(CAPT_COMM_BUT,(Par_Button),CAPT_BUTTON_TOTAL);
    static int clear_button_id = CAPT_BUTTON_TOTAL;
    switch (but_id)
    {
    case CLEAR_BUTTON:
        clear_button_id = CLEAR_BUTTON;
        if(mixed_file_count != 0)
            clear_ack_oper(CLEAR_BUTTON);
        break;
    case CLEAR_ALL_BUTTON:
        clear_button_id = CLEAR_ALL_BUTTON;
        if(mixed_file_count != 0)
            clear_ack_oper(CLEAR_ALL_BUTTON);
        break;
#if SD_CARD
    case SD_BUTTON:
            DEBUG_LOG("--------------->>>SD_BUTTON \n\r");
            if(current_preview_file_type != FILE_TYPE_SD_MIXED)//每次切换文件访问类型，将进入第一页
            mixed_curr_page = 1;

            current_preview_file_type = FILE_TYPE_SD_MIXED;
            button_refresh((Par_Button),ROM_R_6_CAPTURE_SD_ON_PNG,ROM_R_6_CAPTURE_SD_ON_PNG_SIZE,true);
            button_refresh(&CAPT_COMM_BUT[FLASH_BUTTON],ROM_R_6_CAPTURE_FLASH_PNG,ROM_R_6_CAPTURE_FLASH_PNG_SIZE,true);        
            button_disable(&CAPT_COMM_BUT[SD_BUTTON]);
            button_enable(&CAPT_COMM_BUT[FLASH_BUTTON]);   
            mixed_file_list_display(true);        
        break;
    case FLASH_BUTTON:
            DEBUG_LOG("--------------->>>FLASH_BUTTON \n\r");
            if(current_preview_file_type != FILE_TYPE_FLASH_PHOTO)//每次切换文件访问类型，将进入第一页
            mixed_curr_page = 1;

            current_preview_file_type = FILE_TYPE_FLASH_PHOTO;
            button_refresh((Par_Button),ROM_R_6_CAPTURE_FLASH_ON_PNG,ROM_R_6_CAPTURE_FLASH_ON_PNG_SIZE,true);      
            button_disable(&CAPT_COMM_BUT[FLASH_BUTTON]);   
            if(delect_sdcard(false)){
            button_enable(&CAPT_COMM_BUT[SD_BUTTON]);   
            button_refresh(&CAPT_COMM_BUT[SD_BUTTON],ROM_R_6_CAPTURE_SD_PNG,ROM_R_6_CAPTURE_SD_PNG_SIZE,true);
            }
            else
            button_refresh(&CAPT_COMM_BUT[SD_BUTTON],ROM_R_6_CAPTURE_NO_SD_PNG,ROM_R_6_CAPTURE_NO_SD_PNG_SIZE,true);
            mixed_file_list_display(true);        
        break;  
#endif
    case LEFT_BUTTON:
        if(curr_full_screen == FILE_BUTTON_TOTAL){//缩略显示
            mixed_curr_page = mixed_curr_page > 1 ? mixed_curr_page -1 : mixed_tatol_page;
            mixed_file_list_display(false);            
        }else{
            mixed_curr_index = (mixed_curr_index == 0 ? mixed_file_count  : mixed_curr_index) - 1;
            switch_picture();
        }
        break;
    case RIGHT_BUTTON:
        if(curr_full_screen == FILE_BUTTON_TOTAL){
            mixed_curr_page = mixed_curr_page < mixed_tatol_page ? mixed_curr_page +1 : 1;
            mixed_file_list_display(false);
        }else{
            mixed_curr_index = mixed_curr_index == (mixed_file_count-1) ? 0  : mixed_curr_index+ 1;
            switch_picture();
        }
        break;

    case PUASE_BUTTON:
        if(video_play_state_get() == VIDEO_PLAY_ING)
        {
            DEBUG_LOG("video------------->>>pause \n\r");
            is_play_video = false;
            video_play_state_set(VIDEO_PLAY_PAUSE);
            button_display(&CAPT_COMM_BUT[PUASE_BUTTON]);
        }
        else if(video_play_state_get() == VIDEO_PLAY_PAUSE)
        {
            DEBUG_LOG("video------------->>>play \n\r");
            is_play_video = true;            
            video_play_state_set(VIDEO_PLAY_ING);
            gui_erase(&CAPT_COMM_BUT[PUASE_BUTTON].pos,0x00);

        }
        break;       
    case ACK_BUTTON:
        button_control_group(CAPT_COMM_BUT,ACK_BUTTON,CAPT_BUTTON_TOTAL,button_disable);  
        if(curr_full_screen != FILE_BUTTON_TOTAL){//(全屏显示状态)单删除操作
                DEBUG_LOG("single delete file................. :%d\n\r",mixed_curr_index);
                media_file_delete(current_preview_file_type,curr_file_id(mixed_curr_index));   
                mixed_file_count = media_file_total_get(current_preview_file_type,false);
                mixed_curr_index = mixed_curr_index < mixed_file_count ? mixed_curr_index/*  +1 */ : 0;
                if(mixed_file_count == 0){
                    file_button_up(&CAPT_FILE_BUT[SCREE_RETURN_BUTTON]);
                    break;
                }

                confirm_interface(STR_TOTAL,STR_TOTAL,true,0xcc000000,font_size(CAPTURE_BUTTON));
                headline_display(true);
                enable_prev_lay_button(backup_button_group,&backup_button_total);    
                switch_picture();
        }else{
            if(clear_button_id == CLEAR_BUTTON)
            {
                for(int i = mixed_file_count -1 ;i > -1;i--)
                {
                    if(select_file[i]){
                        DEBUG_LOG("media_file %d delete \n\r",i);
                        media_file_delete(current_preview_file_type,i);            
                    }
                } 
            }
            else//(缩略图显示状态)全删除操作
            {
                DEBUG_LOG("all delete file..................\n\r");
            #if SD_CARD
                    if(current_preview_file_type == FILE_TYPE_SD_MIXED)
                    start_delete_media(DELETE_ALL_MIXED);
                    else
            #endif
                    photo_file_all_clear(current_preview_file_type);
            }
            mixed_file_list_display(true);
            file_button_up(&CAPT_FILE_BUT[SCREE_RETURN_BUTTON]);
            clear_button_id = CAPT_BUTTON_TOTAL;
        }
        break;
    case CANCEL_BUTTON:
        confirm_interface(STR_TOTAL,STR_TOTAL,true,0xcc000000,font_size(CAPTURE_BUTTON));
        headline_display(true);
        enable_prev_lay_button(backup_button_group,&backup_button_total);    
        button_control_group(CAPT_COMM_BUT,ACK_BUTTON,CAPT_BUTTON_TOTAL,button_disable);  
        if(curr_full_screen != FILE_BUTTON_TOTAL){//(全屏显示状态)
            curr_serial_display();
            position pos = {{0,0},{1024,600}};
            file_display(pos,mixed_curr_index,false);
        }else{
            mixed_file_list_display(true);
        }
        if(video_play_state_get() == VIDEO_PLAY_PAUSE)
            button_display(&CAPT_COMM_BUT[PUASE_BUTTON]);     

        break;

    default:
        break;
    }

}
static void common_button_down(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int but_id = SequenceSearch(CAPT_COMM_BUT,(Par_Button),CAPT_BUTTON_TOTAL);
    switch (but_id)
    {
    case CLEAR_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_CLEAR_SELECT_PNG,ROM_R_6_CAPTURE_CLEAR_SELECT_PNG_SIZE,true);
        break;
     case CLEAR_ALL_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_CLEAR_ALL_SELECT_PNG,ROM_R_6_CAPTURE_CLEAR_ALL_SELECT_PNG_SIZE,true);
        break;
    case LEFT_BUTTON:
        // button_refresh((Par_Button),ROM_R_6_CAPTURE_LEFT_N_PNG,ROM_R_6_CAPTURE_LEFT_N_PNG_SIZE,true);
        break;
    case RIGHT_BUTTON:
        // button_refresh((Par_Button),ROM_R_6_CAPTURE_RIGHT_N_PNG,ROM_R_6_CAPTURE_RIGHT_N_PNG_SIZE,true);
        break;
    case ACK_BUTTON:
    case CANCEL_BUTTON:
        button_refresh((Par_Button),ROM_R_2_SET_ACK_BUTTON_ON_PNG,ROM_R_2_SET_ACK_BUTTON_ON_PNG_SIZE,true);
        break;      
    default:
        break;
    }
}
static void common_button_leave_down(const void *arg)
{
    DEBUG_LOG("--------------->>>%s \n\r",__func__);
    int but_id = SequenceSearch(CAPT_COMM_BUT,(Par_Button),CAPT_BUTTON_TOTAL);
    switch (but_id)
    {
    case CLEAR_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_CLEAR_PNG,ROM_R_6_CAPTURE_CLEAR_PNG_SIZE,true);
        break;
     case CLEAR_ALL_BUTTON:
        button_refresh((Par_Button),ROM_R_6_CAPTURE_CLEAR_ALL_PNG,ROM_R_6_CAPTURE_CLEAR_ALL_PNG_SIZE,true);
        break;
    case LEFT_BUTTON:
        // button_refresh((Par_Button),ROM_R_6_CAPTURE_LEFT_PNG,ROM_R_6_CAPTURE_LEFT_PNG_SIZE,true);
        break;
    case RIGHT_BUTTON:
        // button_refresh((Par_Button),ROM_R_6_CAPTURE_RIGHT_PNG,ROM_R_6_CAPTURE_RIGHT_PNG_SIZE,true);
        break;
    case ACK_BUTTON:
    case CANCEL_BUTTON:
    button_refresh((Par_Button),ROM_R_2_SET_ACK_BUTTON_PNG,ROM_R_2_SET_ACK_BUTTON_PNG_SIZE,true);
        break;       
    default:
        break;
    }

}
/* 操作按键-初始化 */
static void common_button_init(button *BUTTON,position pos,resource res,bool icon)
{
    button_init(BUTTON,&pos,common_button_up);
    BUTTON->down = common_button_down;
    BUTTON->leave_down = common_button_leave_down;
    if(icon)
    button_icon_init(BUTTON,&res);
}
static void common_button_list(void)
{
    {
        position pos = {{688,26},{64,64}};

#if SD_CARD     
        #if current_preview_file_type ==  FILE_TYPE_FLASH_PHOTO
        resource res_f= resource_get(ROM_R_6_CAPTURE_FLASH_ON_PNG);    
        #else
        resource res_f= resource_get(ROM_R_6_CAPTURE_FLASH_PNG);   
        #endif
        common_button_init(&CAPT_COMM_BUT[FLASH_BUTTON],pos,res_f,true);     


        resource res_s= resource_get(ROM_R_6_CAPTURE_SD_PNG);    
        pos.point.x = 764;
        common_button_init(&CAPT_COMM_BUT[SD_BUTTON],pos,res_s,true);       
#endif     
        resource res_a= resource_get(ROM_R_6_CAPTURE_CLEAR_ALL_PNG);  
        pos.point.x = 842;
        common_button_init(&CAPT_COMM_BUT[CLEAR_ALL_BUTTON],pos,res_a,true);     

        resource res_c= resource_get(ROM_R_6_CAPTURE_CLEAR_PNG);               
        pos.point.x = 918;
        common_button_init(&CAPT_COMM_BUT[CLEAR_BUTTON],pos,res_c,true);    
        CAPT_COMM_BUT[CLEAR_BUTTON].bg_color = BG_COLOR;
    }

    {
        position pos = {{1,280},{50,100}};    
        resource res_l= resource_get(ROM_R_6_CAPTURE_LEFT_N_PNG);
        resource res_r = resource_get(ROM_R_6_CAPTURE_RIGHT_N_PNG);
        common_button_init(&CAPT_COMM_BUT[LEFT_BUTTON],pos,res_l,true);      
        // CAPT_COMM_BUT[LEFT_BUTTON].bg_color = BLUE;     
        pos.point.x = 981;
        common_button_init(&CAPT_COMM_BUT[RIGHT_BUTTON],pos,res_r,true);     
        // CAPT_COMM_BUT[RIGHT_BUTTON].bg_color = BLUE;         
    }

    {
        position pos = {{0,100},{1024,400}};          
        resource res = resource_get(ROM_R_6_CAPTURE_PUASE_PNG);
        common_button_init(&CAPT_COMM_BUT[PUASE_BUTTON],pos,res,true);          
    }

    {
        position pos = {{249,341},{210,60}};    
        resource res_yes= resource_get(ROM_R_2_SET_ACK_BUTTON_PNG);
        resource res_no = resource_get(ROM_R_2_SET_ACK_BUTTON_PNG);
        common_button_init(&CAPT_COMM_BUT[ACK_BUTTON],pos,res_yes,true);   
        // CAPT_COMM_BUT[ACK_BUTTON].bg_color = 0xff3F57CF;
        // CAPT_COMM_BUT[ACK_BUTTON].font_color = RED;
        pos.point.x = 552;
        common_button_init(&CAPT_COMM_BUT[CANCEL_BUTTON],pos,res_no,true);         
        button_text_init(&CAPT_COMM_BUT[ACK_BUTTON],btn_str(STR_YES),font_size(CAPTURE_INFO) - 4);
        button_text_init(&CAPT_COMM_BUT[CANCEL_BUTTON],btn_str(STR_NO),font_size(CAPTURE_INFO) - 4);
    }  

}

static void capture_button_display(void)
{
    goto_home_button_display(true);
    button_display(&goto_home_button);
    button_control_group(CAPT_COMM_BUT,CLEAR_ALL_BUTTON,FLASH_BUTTON,button_display);       
}

static void capture_button_enable(void)
{
    button_enable(&goto_home_button);
    button_control_group(CAPT_COMM_BUT,CLEAR_ALL_BUTTON,PUASE_BUTTON,button_enable);       
}

static void capture_button_open(void){
    capture_button_display();
    capture_button_enable();
}
static void layout_capture_init(void)
{
    goto_home_button_init();
    common_button_list();
    file_button_list();
}
#if SD_CARD   
/* 文件访问-操作
    is_sd_insert：SD卡是否插入 */
static void memory_status_action(bool is_sd_insert)
{
    if(is_sd_insert)
    {
        if(current_preview_file_type == FILE_TYPE_SD_MIXED)
        common_button_up(&CAPT_COMM_BUT[SD_BUTTON]);
        else
        common_button_up(&CAPT_COMM_BUT[FLASH_BUTTON]);
    }
    else
    {
        common_button_up(&CAPT_COMM_BUT[FLASH_BUTTON]);
        button_disable(&CAPT_COMM_BUT[SD_BUTTON]);
    }
}
#endif
static void layout_capture_enter(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);   
    CAPT_COMM_BUT[LEFT_BUTTON].bg_color = CAPT_COMM_BUT[RIGHT_BUTTON].bg_color = LUCE_BLACK;
    if(media_file_total_get(current_preview_file_type,false) != 0)
    {
        thumb_device_open();
        network_h264_thumb_device_open();
        mixed_file_list_display(true);          
    }
    else
    {
        get_file_page();
        page_display();
    }
    capture_button_open();
    headline_display(false);
#if SD_CARD   
    memory_status_action(is_sdcard_insert());
#else
#endif
}

static void layout_capture_quit(void)
{
    // DEBUG_LOG("--------------->>>%s \n\r",__func__);
    is_play_video = false;
    curr_full_screen = FILE_BUTTON_TOTAL;
    mixed_curr_page = 1;
    thumb_device_close();
    network_h264_thumb_device_close();
    if(select_file != NULL)
    {
        free(select_file);
        select_file = NULL;
    }
}
static void layout_capture_timer(void)
{
    //DEBUG_LOG("--------------->>>%s \n\r",__func__);
    static unsigned long long pre_pts = 0;
    unsigned long  long pts = os_get_ms();
    int cur_puase_time = 0;
    int total_puase_time = 0;
    if((pts - pre_pts)> 100)
    {
        pre_pts = pts;
#if SD_CARD
        bool is_sd_plug = delect_sdcard(false);
        if((!is_sd_plug)&&(is_sd_plug != sd_curr_status))
        {
            sd_curr_status = is_sd_plug;
            DEBUG_LOG("is_sd_plug-------------->>>false \n\r");
            if(current_preview_file_type == FILE_TYPE_SD_MIXED)
            {
                position pos = {{0,90},{1024,420}};
                file_but_cont_group(CAPT_FILE_BUT,FILE_1_BUTTON,SCREE_RETURN_BUTTON,button_disable);      
                // wallpaper_dispaly(IMG_BLACK);
                set_gui_background_color(MAIN_BG_COLOUR);
                gui_erase(&pos,0x00);
                
            }         

            if((curr_full_screen != FILE_BUTTON_TOTAL)&&current_preview_file_type != FILE_TYPE_FLASH_PHOTO)
            {
                if(video_play_state_get() == VIDEO_PLAY_ING)
                video_play_stop();

                file_button_up(&CAPT_FILE_BUT[SCREE_RETURN_BUTTON]);             
            }
            else if(curr_full_screen == FILE_BUTTON_TOTAL){
                memory_status_action(false);    
            }
        }

        if(is_sd_plug&&(is_sd_plug != sd_curr_status))
        {
            DEBUG_LOG("is_sd_plug-------------->>>ture \n\r");
            sd_curr_status = is_sd_plug;
            if(curr_full_screen == FILE_BUTTON_TOTAL)
            {    
                memory_status_action(true);         
            }
        }
#endif

        if(is_play_video)
        {
            if(video_play_state_get() == VIDEO_PLAY_PAUSE){
            button_display(&CAPT_COMM_BUT[PUASE_BUTTON]);
            is_play_video = false;
            }
            static int pre_puase_time = 0;
            video_duration_get(&cur_puase_time,&total_puase_time);
            if((total_puase_time >= cur_puase_time)&&abs(cur_puase_time - pre_puase_time)>=100){
                pre_puase_time = cur_puase_time;
                video_play_progress_display(cur_puase_time,total_puase_time);                
            }
        }      
    }
}
layout layout_capture = 
{
    .init = layout_capture_init,
    .enter = layout_capture_enter,
    .quit = layout_capture_quit,
    .timer = layout_capture_timer
};