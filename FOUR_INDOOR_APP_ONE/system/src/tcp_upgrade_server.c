/*server.c*/
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "tcp_upgrade.h"
#include "ak_thread.h"

#define FILE_MAX_LEN 256

static int remove_dir(const char *path) {

    char filename[FILE_MAX_LEN] = {0};
    struct dirent *dirp;
    struct stat statbuf;
    DIR *dp = NULL;
    dp = opendir(path);
    if (NULL == dp) {
        return -1;
    }
    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
            continue;
        }
        sprintf(filename, "%s%s", path, dirp->d_name);
        if (!stat(filename, &statbuf)) {
            unlink(filename);
        }
    }
    closedir(dp);
    return 0;
}

#define HELLO_WORLD_SERVER_PORT    6666         //端口号
#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024
#define FILE_NAME_MAX_SIZE         512

#define TF_UPGRADE_PACKAGE_PATH  "/mnt/tf/"
#define NET_CAMERA_UPDATE_FILE   "/mnt/tf/four_wire_camera.update"
#define TMP_LOCAL_UPDATE_DIR     "/tmp/four_wire_camera/"

#define UNZIP_DST_PATH       "/tmp/"

#define UPGRADE_FILE_DIR     "four_wire_camera/"
#define UPGRADE_FILE_EXE     "four_wire_camera/net_camera"
#define UPGRADE_FILE_VER     "four_wire_camera/version_"
#define UPGRADE_FILE_SHA1    "four_wire_camera/net_camera.sha1"

#define UPGRADE_FILE_VER_FLAG "version_"
#define FILE_SIZE_FLAG        "file_size:"

#define SHA1SUM_OKAY_FLAG     "sha1sum:Okay"
#define SHA1SUM_FAIL_FLAG     "sha1sum:Fail"

tcp_status tcp_status_flag = TUF_EMPTY;

#if 0
static void printdir(char *dir, int depth) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
                continue;
            }
            printf("1%*s%s/\n", depth, " ", entry->d_name);
            printdir(entry->d_name, depth + 4);
        } else {
            printf("2%*s%s\n", depth, " ", entry->d_name);
            if (strncmp(entry->d_name, UPGRADE_FILE_VER_FLAG, strlen(UPGRADE_FILE_VER_FLAG)) == 0){
                char *p = strchr(entry->d_name, '_') + 1;
                char *stop_str;
                printf("version: %lu\n\r", strtol(p,&stop_str, 0));
            }
        }
    }
    chdir("..");
    closedir(dp);
}
#endif

bool server_update_file_exist(void) {
    if (access(NET_CAMERA_UPDATE_FILE, F_OK) == 0) {
        printf("1.detected update file.\n\r");
        tcp_status_flag = TUF_EXISTS;
        return true;
    }
    tcp_status_flag = TUF_EMPTY;
    return false;
}

static int tar_xzvf_local_update_file(void) {

    remove_dir(TMP_LOCAL_UPDATE_DIR);

    char cmd[512] = {0};
    sprintf(cmd, "tar -xzvf %s -C %s ", NET_CAMERA_UPDATE_FILE, UNZIP_DST_PATH);
    int ret = 0;
    FILE *pf = popen(cmd,"r");
    if(pf == NULL){
        perror("\t Unzip the upgrade file fail!\n\r");
        return ret;
    }
    printf("2.Unzip the upgrade file.\n\r");
    char buffer[256] = {0};
    memset(buffer,0,sizeof (buffer));
    while(fgets(buffer,sizeof (buffer),pf)){
        printf("\t file: %s \n\r",buffer);
        char *p_dir = strchr(buffer, 'f');
        if(strncmp(p_dir,UPGRADE_FILE_DIR, strlen(UPGRADE_FILE_DIR)) == 0){
            ret += 1;
        }else if(strncmp(p_dir,UPGRADE_FILE_EXE, strlen(UPGRADE_FILE_EXE)) == 0){
            ret += 1;
        }else if(strncmp(p_dir,UPGRADE_FILE_VER, strlen(UPGRADE_FILE_VER)) == 0){
            ret += 1;
        }else if(strncmp(p_dir,UPGRADE_FILE_SHA1, strlen(UPGRADE_FILE_SHA1)) == 0){
            ret += 1;
        }
    }
    printf("\t A total of %d upgrade files were found.\n\r",ret);
    pclose(pf);
    pf = NULL;
    return ret;
}
static bool local_update_file_sha1sum_check(void){
    bool ret = false;
    printf("3.Start file integrity check.\n\r");
    char cmd[128] = {0};
    memset(cmd,0,sizeof (cmd));
    sprintf(cmd,"sha1sum %s%s",UNZIP_DST_PATH,UPGRADE_FILE_EXE);
    FILE *pf = popen(cmd,"r");
    if(pf == NULL){
        perror("\t Start file integrity check fail!\n\r");
        return ret;
    }
    char buffer[256] = {0};
    memset(buffer,0,sizeof (buffer));
    while(fgets(buffer,sizeof (buffer),pf)){
        //printf("buffer: %s \n\r",buffer);
        //if(strncmp(buffer,"/tmp/net_camera/net_camera",26) == 0)
        char *p = strchr(buffer, ' ');
        char hash[128] = {0};
        strncpy(hash,buffer, p - buffer);
        printf("\t Current hash: %s \n\r", hash);
        ret = true;
    }
    pclose(pf);
    pf = NULL;

    char correct_hash_buff[255] = {0};
    char file_path[128] = {0};
    sprintf(file_path,"%s%s",UNZIP_DST_PATH,UPGRADE_FILE_SHA1);

    pf = fopen(file_path, "r");
    fscanf(pf,"%s",correct_hash_buff);
    printf("\t Correct hash: %s \n\r",correct_hash_buff);
    fclose(pf);
    pf = NULL;
    if(strncmp(correct_hash_buff,buffer,strlen(correct_hash_buff)) == 0){
        ret = true;
        printf("\t Hash check passed.\n\r");
    }
    return ret;
}
static unsigned long get_local_tmp_update_file_version(char *dir, int depth) {

    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return 0;
    }
    chdir(dir);//将当前工作目录改变为dir目录
    unsigned long version = 0;

    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
                continue;
            }
            printf("\t %*s%s/\n", depth, " ", entry->d_name);
            version = get_local_tmp_update_file_version(entry->d_name, depth + 4);
        } else {
            //printf("\t %*s%s\n", depth, " ", entry->d_name);
            if (strncmp(entry->d_name, UPGRADE_FILE_VER_FLAG, strlen(UPGRADE_FILE_VER_FLAG)) == 0){
                char *p = strchr(entry->d_name, '_') + 1;
                char *stop_str;
                version = strtol(p,&stop_str, 0);
                printf("\t Current version: %lu\n\r", version);
                break;
            }
        }
    }
    chdir("..");
    closedir(dp);
    return version;
}
static unsigned long get_remote_current_version(char *version_str){
    unsigned long version = 0;
    if (strncmp(version_str, UPGRADE_FILE_VER_FLAG, strlen(UPGRADE_FILE_VER_FLAG)) == 0){
        char *p = strchr(version_str, '_') + 1;
        char *stop_str;
        version = strtol(p,&stop_str, 0);
        printf("\t Remote version: %lu\n\r", version);
    }
    return version;
}
static int file_size_get(FILE* fp){
    fseek(fp,0,SEEK_END);
    int file_size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    return file_size;
}

static void printf_progress(int percentage) {

    // char buff[128] = {0};
    // int str_len = 0;
    printf("%d%%", percentage);
    putc('\b', stdout);
    putc('\b', stdout);
    putc('\b', stdout);
    fflush(stdout);
}

static int server_socket = 0;
bool create_upgrade_tcp_server(void){
    // set socket's address information
    // 设置一个socket地址结构server_addr,代表服务器internet的地址和端口
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);

    // create a stream socket
    // 创建用于internet的流协议(TCP)socket，用server_socket代表服务器向客户端提供服务的接口
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Create Socket Failed!\n");
        return false;
    }

    // 把socket和socket地址结构绑定
    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        printf("Server Bind Port: %d Failed!\n", HELLO_WORLD_SERVER_PORT);
        return false;
    }

    // server_socket用于监听
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE)) {
        printf("Server Listen Failed!\n");
        return false;
    }
    return true;
}
#define STEP_START 0
#define STEP_UPLOAD_VERSION  1
#define STEP_PUSH_FILE_SIZE  2
#define STEP_WAIT_REMOTE_FILE_INTEGRITY_CHECK 3
#define STEP_STOP 4

static int upgrade_step_flag = STEP_START;
static bool has_upgrade_file = false;
static bool complete_upgrade_file = false;

static void clear_tmp_flag(void){
    upgrade_step_flag = STEP_START;
    has_upgrade_file = false;
    complete_upgrade_file = false;
}

static ak_pthread_t upgrade_server_thread_id;
static bool upgrade_server_thread_run = false;
static bool upgrade_outdoor_finish = false;

bool upgrade_outdoor_status(void){
    return upgrade_outdoor_finish;
}
void set_upgrade_outdoor_status(bool status){
    upgrade_outdoor_finish = status;
}

static void *upgrade_server_task(void *arg) {

    unsigned long current_version = 0;
    unsigned long update_version = 0;

    clear_tmp_flag();

    create_upgrade_tcp_server();

    int new_server_socket = -1;
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    // 服务器端一直运行用以持续为客户端提供服务
    tcp_status_flag = TUF_EMPTY;

    bool *run = (bool *) arg;
    printf("------------------------------------>%s\n\r",__func__);
    while (*run) {
        if(new_server_socket == -1){
            // 定义客户端的socket地址结构client_addr，当收到来自客户端的请求后，调用accept
            // 接受此请求，同时将client端的地址和端口等信息写入client_addr中
            // 接受一个从client端到达server端的连接请求,将客户端的信息保存在client_addr中
            // 如果没有连接请求，则一直等待直到有连接请求为止，这是accept函数的特性，可以
            // 用select()来实现超时检测
            // accpet返回一个新的socket,这个socket用来与此次连接到server的client进行通信
            // 这里的new_server_socket代表了这个通信通道
            printf("0.Waiting for client to connect.\n\r");
            new_server_socket = accept(server_socket, (struct sockaddr *) &client_addr, &length);
            printf("0.Client to connect succeed\n\r");
            if (new_server_socket < 0) {
                printf("Server Accept Failed!\n");
                break;
            }
        }

        char buffer[BUFFER_SIZE];
        bzero(buffer, sizeof(buffer));
        length = 0;
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);
        printf("lenght---------------->%d\n\r",length);
        if (length < 0) {
            printf("Server Recieve Data Failed!\n");
            break;
        }
        if(upgrade_step_flag == STEP_START && length > 0){

            has_upgrade_file = server_update_file_exist();//判断TF卡是否存在更新压缩包
            if(has_upgrade_file){
                if(tar_xzvf_local_update_file() > 0){//解压到/tmp/
                    complete_upgrade_file = local_update_file_sha1sum_check();//判断文件是否完整
                }else{
                    tcp_status_flag = TUF_EMPTY;
                }
            }else{
                tcp_status_flag = TUF_EMPTY;
            }
            if(complete_upgrade_file){
                printf("4.Get upgrade file version.\n\r");
                tcp_status_flag = TUF_EXISTS;
                current_version = get_remote_current_version(buffer);
                update_version = get_local_tmp_update_file_version(TMP_LOCAL_UPDATE_DIR,0);
                if(current_version && update_version){
                    if(update_version > current_version) {
                        tcp_status_flag = TUF_UPDATE;
                        printf("\t Upgrade to the new version[%lu].\n\r",update_version);
                    }else{
                        tcp_status_flag = TUF_ROLL_BACK;
                        printf("\t Roll back to an older version[%lu].\n\r",update_version);
                    }
                    upgrade_step_flag = STEP_UPLOAD_VERSION;
                }else{
                    if(current_version == 0 && update_version){
                        tcp_status_flag = TUF_UPDATE;
                        upgrade_step_flag = STEP_UPLOAD_VERSION;
                        printf("\t Upgrade to the new version[%lu].\n\r",update_version);
                    }else{
                        tcp_status_flag = TUF_INVALID_VERSION;
                        printf("\t Invalid version[Remote:%lu][Local:%lu].\n\r",current_version,update_version);
                    }
                }
            }else{
                tcp_status_flag = TUF_DAMAGED;
            }
        }else if(upgrade_step_flag == STEP_UPLOAD_VERSION){
            char file_name[FILE_NAME_MAX_SIZE + 1];
            bzero(file_name, sizeof(file_name));
            //strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
            sprintf(file_name,"%s%s", TF_UPGRADE_PACKAGE_PATH,buffer);
            printf("5.Push the upgrade file : %s \n\r",file_name);
            upgrade_step_flag = STEP_PUSH_FILE_SIZE;

            FILE *fp = fopen(file_name, "r");
            if (fp == NULL) {
                printf("\t File:\t%s Not Found!\n", file_name);
            } else {

                int file_size  = file_size_get(fp);
                bzero(buffer, sizeof(buffer));
                sprintf(buffer,"%s%d",FILE_SIZE_FLAG,file_size);
                if (send(new_server_socket, buffer, BUFFER_SIZE, 0) < 0) {
                    printf("\t Send file size %d fail.\n\r",file_size);
                    tcp_status_flag = TUF_TRANSFER_FAIL;
                }else{
                    tcp_status_flag = TUF_TRANSFER;
                    printf("\t File size %d byte.\n\r",file_size);
                }

                bzero(buffer, BUFFER_SIZE);
                int file_block_length = 0;
                int has_send_size = 0;
                printf("\n\r\t Has been sent:   ");
                while ((file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) {
                    //printf("\t Has been sent = %d byte.\n", file_block_length);
                    // 发送buffer中的字符串到new_server_socket,实际上就是发送给客户端
                    if (send(new_server_socket, buffer, file_block_length, 0) < 0) {
                        printf("\n\t Send File:\t%s Failed!\n", file_name);
                        upgrade_step_flag = STEP_STOP;
                        tcp_status_flag = TUF_TRANSFER_FAIL;
                        break;
                    }else{
                        has_send_size += file_block_length;
                        printf_progress(has_send_size*100/file_size);
                    }
                    bzero(buffer, sizeof(buffer));
                }
                fclose(fp);
                upgrade_step_flag = STEP_WAIT_REMOTE_FILE_INTEGRITY_CHECK;
                printf("\n\t File:\t%s Transfer Finished!\n", file_name);
            }
        }else if(upgrade_step_flag == STEP_WAIT_REMOTE_FILE_INTEGRITY_CHECK){
            if(length > 0){
                if (strncmp(buffer, SHA1SUM_OKAY_FLAG, strlen(SHA1SUM_OKAY_FLAG)) == 0){
                    printf("6. File integrity check passed.\n\r");
                    upgrade_step_flag = STEP_STOP;
                    tcp_status_flag = TUF_FINISH;
                    set_upgrade_outdoor_status(true);
                }else if(strncmp(buffer, SHA1SUM_FAIL_FLAG, strlen(SHA1SUM_FAIL_FLAG)) == 0){
                    printf("6. File integrity check failed.\n\r");
                    upgrade_step_flag = STEP_STOP;
                    tcp_status_flag = TUF_TRANSFER_DAMAGED;
                }
            }
        }
        if(upgrade_step_flag == STEP_STOP){
            if(new_server_socket != -1){
                close(new_server_socket);
                new_server_socket = -1;
                // *run = false;
            }
            tcp_status_flag = TUF_EMPTY;
            clear_tmp_flag();
        }
    }
    if(new_server_socket != -1){
        close(new_server_socket);
    }
    close(server_socket);
    ak_thread_join(upgrade_server_thread_id);
    upgrade_server_thread_id = 0;
    printf("%s quit...............\n\r",__func__);
    return 0;
}

tcp_status upgrade_server_status(void){
    return tcp_status_flag;
}

bool create_upgrade_server_task(void) {
    if(upgrade_server_thread_run){
        return false;
    }
    upgrade_server_thread_run = true;
    ak_thread_create(&upgrade_server_thread_id, upgrade_server_task, &upgrade_server_thread_run, ANYKA_THREAD_NORMAL_STACK_SIZE, -1);
    return true;
}

bool destroy_upgrade_server_task(void) {
    if (upgrade_server_thread_run == false) {
        return false;
    }
    upgrade_server_thread_run = false;
    ak_thread_join(upgrade_server_thread_id);
    upgrade_server_thread_id = 0;
    return true;
}