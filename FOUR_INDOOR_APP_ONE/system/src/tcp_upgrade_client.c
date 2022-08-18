/*client.c*/
#include <netinet/in.h>                         // for sockaddr_in
#include <sys/socket.h>                         // for socket
#include <stdio.h>                              // for printf
#include <stdlib.h>                             // for exit
#include <string.h>                             // for bzero
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <arpa/inet.h>
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

#define HELLO_WORLD_SERVER_PORT       6666
#define BUFFER_SIZE                   1024
#define FILE_NAME_MAX_SIZE            512

#define LOCAL_UPDATE_DIR            "/etc/jffs2/cbin/" // "/etc/config/net_camera/"
#define PULL_UPGRADE_FILE_NAME      "four_wire_camera.update"
#define UPGRADE_PACKAGE_PATH        "/etc/jffs2/four_wire_camera.update"

#define UNZIP_DST_PATH       "/tmp/"

#define UPGRADE_FILE_DIR     "net_camera/"
#define UPGRADE_FILE_EXE     "net_camera/net_camera"
#define UPGRADE_FILE_VER     "net_camera/version_"
#define UPGRADE_FILE_SHA1    "net_camera/net_camera.sha1"

#define UPGRADE_FILE_VER_FLAG "version_"
#define FILE_SIZE_FLAG        "file_size:"

#define SHA1SUM_OKAY_FLAG     "sha1sum:Okay"
#define SHA1SUM_FAIL_FLAG     "sha1sum:Fail"

static unsigned long get_dir_update_file_version(char *dir, int depth, char *version_str) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return 0;
    }
    chdir(dir);
    unsigned long version = 0;

    printf("1.Get current upgrade file version.\n\r");

    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);//获取文件属性
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
                continue;
            }
            printf("\t %*s%s/\n", depth, " ", entry->d_name);
            get_dir_update_file_version(entry->d_name, depth + 4, version_str);
        } else {
            printf("\t %*s%s\n", depth, " ", entry->d_name);
            if (strncmp(entry->d_name, UPGRADE_FILE_VER_FLAG, strlen(UPGRADE_FILE_VER_FLAG)) == 0) {
                char *p = strchr(entry->d_name, '_') + 1;
                char *stop_str;
                version = strtol(p, &stop_str, 0);
                printf("\t  version: %lu\n\r", version);
                if (version_str != NULL) {
                    sprintf(version_str, "%s", entry->d_name);
                }
                break;
            }
        }
    }
    chdir("..");
    closedir(dp);
    return version;
}

static int tar_xzvf_local_update_file(void) {

    char tmp_unzip_path[128] = {0};
    sprintf(tmp_unzip_path, "%s%s", UNZIP_DST_PATH, UPGRADE_FILE_DIR);
    if (access(tmp_unzip_path, F_OK) == 0) {
        remove_dir(tmp_unzip_path);
    }

    char cmd[512] = {0};
    sprintf(cmd, "tar -xvf %s -C %s ", UPGRADE_PACKAGE_PATH, UNZIP_DST_PATH);
    int ret = 0;
    FILE *pf = popen(cmd, "r");
    if (pf == NULL) {
        perror("Unzip the upgrade file fail!\n\r");
        return ret;
    }
    printf("5.Unzip the upgrade file.\n\r");
    char buffer[256] = {0};
    memset(buffer, 0, sizeof(buffer));
    while (fgets(buffer, sizeof(buffer), pf)) {
        printf("\t file: %s \n\r", buffer);
        if (strncmp(buffer, UPGRADE_FILE_DIR, strlen(UPGRADE_FILE_DIR)) == 0) {
            ret += 1;
        } else if (strncmp(buffer, UPGRADE_FILE_EXE, strlen(UPGRADE_FILE_EXE)) == 0) {
            ret += 1;
        } else if (strncmp(buffer, UPGRADE_FILE_VER, strlen(UPGRADE_FILE_VER)) == 0) {
            ret += 1;
        } else if (strncmp(buffer, UPGRADE_FILE_SHA1, strlen(UPGRADE_FILE_SHA1)) == 0) {
            ret += 1;
        }
    }
    printf("\t A total of %d upgrade files were found.\n\r", ret);
    pclose(pf);
    pf = NULL;
    return ret;
}

static bool check_camera_update_file(void) {
    if (access(UPGRADE_PACKAGE_PATH, F_OK) == 0) {
        printf("4.detected update file.\n\r");
        return true;
    }
    return false;
}

static bool local_update_file_sha1sum_check(void) {
    bool ret = false;
    printf("6.Start file integrity check.\n\r");
    char buffer[256] = {0};
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "sha1sum %s%s", UNZIP_DST_PATH, UPGRADE_FILE_EXE);
    FILE *pf = popen(buffer, "r");
    if (pf == NULL) {
        perror("\t Start file integrity check fail!\n\r");
        return ret;
    }

    memset(buffer, 0, sizeof(buffer));
    while (fgets(buffer, sizeof(buffer), pf)) {
        //printf("buffer: %s \n\r",buffer);
        //if(strncmp(buffer,"/tmp/net_camera/net_camera",26) == 0)
        char *p = strchr(buffer, ' ');
        char hash[128] = {0};
        strncpy(hash, buffer, p - buffer);
        printf("\t Current hash: %s \n\r", hash);
        ret = true;
    }
    pclose(pf);
    pf = NULL;

    char correct_hash_buff[255] = {0};
    char file_path[128] = {0};
    sprintf(file_path, "%s%s", UNZIP_DST_PATH, UPGRADE_FILE_SHA1);
    pf = fopen(file_path, "r");
    memset(file_path, 0, sizeof(file_path));
    fscanf(pf, "%s", correct_hash_buff);
    printf("\t Correct hash: %s \n\r", correct_hash_buff);
    fclose(pf);
    pf = NULL;
    if (strncmp(correct_hash_buff, buffer, strlen(correct_hash_buff)) == 0) {
        ret = true;
        printf("\t Hash check passed.\n\r");
    }
    return ret;
}

static bool copy_upgrade_file(void) {
    char cmd[512] = {0};
    sprintf(cmd, "cp -rf  %s%s* %s  ", UNZIP_DST_PATH, UPGRADE_FILE_DIR, LOCAL_UPDATE_DIR);

    FILE *pf = popen(cmd, "r");
    if (pf == NULL) {
        perror("\t Copy upgrade file fail!\n\r");
        return false;
    } else {
        printf("7.Update successed.\n\r");
    }
    pclose(pf);
    pf = NULL;
    return true;
}

static void *connect_server_task(void *arg) {
    // int ret = 0;
    char *upgrade_client_ip = (char *) arg;
    if (upgrade_client_ip == NULL) {
        printf("Upgrade server ip is NULL.\n");
        return NULL;
    }

    // 设置一个socket地址结构client_addr, 代表客户机的internet地址和端口
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET; // internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY表示自动获取本机地址
    client_addr.sin_port = htons(0); // auto allocated, 让系统自动分配一个空闲端口

    // 创建用于internet的流协议(TCP)类型socket，用client_socket代表客户端socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("Create Socket Failed!\n");
        return NULL;
    }

    // 把客户端的socket和客户端的socket地址结构绑定
    if (bind(client_socket, (struct sockaddr *) &client_addr, sizeof(client_addr))) {
        printf("Client Bind Port Failed!\n");
        return NULL;
    }

    // 设置一个socket地址结构server_addr,代表服务器的internet地址和端口
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // 服务器的IP地址来自程序的参数
    if (inet_aton(upgrade_client_ip, &server_addr.sin_addr) == 0) {
        printf("Server IP Address Error!\n");
        return NULL;
    }

    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);

    // 向服务器发起连接请求，连接成功后client_socket代表客户端和服务器端的一个socket连接
    if (connect(client_socket, (struct sockaddr *) &server_addr, server_addr_length) < 0) {
        printf("Can Not Connect To %s!\n", (char *)upgrade_client_ip);
        return NULL;
    }

    // 向服务器发送当前软件版本
    char buffer[BUFFER_SIZE];
    bzero(buffer, sizeof(buffer));
    get_dir_update_file_version(LOCAL_UPDATE_DIR, 0, buffer);
    //sprintf(buffer, "%s","version_010");
    send(client_socket, buffer, BUFFER_SIZE, 0);

    // 向服务器发送buffer中的数据，此时buffer中存放的是客户端需要接收的文件的名字
    char file_name[FILE_NAME_MAX_SIZE + 1];
    bzero(file_name, sizeof(file_name));
    //printf("Please Input File Name On Server.\t");
    //scanf("%s", file_name);
    sprintf(file_name, "%s", PULL_UPGRADE_FILE_NAME);
    printf("2.Upload upgrade filename.\n\r");
    bzero(buffer, sizeof(buffer));
    strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
    send(client_socket, buffer, BUFFER_SIZE, 0);

    int length = 0;
    unsigned long update_file_size = 0;
    length = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (length > 0) {
        if (strncmp(buffer, FILE_SIZE_FLAG, strlen(FILE_SIZE_FLAG)) == 0) {
            char *p = strchr(buffer, ':') + 1;
            char *stop_str;
            update_file_size = strtol(p, &stop_str, 0);
            printf("\t Current upgrade file size: %lu Byte.\n\r", update_file_size);
        }
    }

    //printf("3.Pull remote upgrade files.\n\r");
    //bzero(file_name, sizeof(file_name));
    //sprintf(file_name,"%s",UPGRADE_PACKAGE_PATH);

    FILE *fp = fopen(file_name, "w+");
    if (fp == NULL) {
        printf(" \t File: %s Can Not Open To Write!\n", file_name);
        return NULL;
    }
    // 从服务器端接收数据到buffer中
    bzero(buffer, sizeof(buffer));
    int received_length = 0;
    while ((length = recv(client_socket, buffer, BUFFER_SIZE, 0))) {
        if (length < 0) {
            printf("3.Recieve Data From Server %s Failed!\n", (char *)upgrade_client_ip);
            break;
        }

        int write_length = fwrite(buffer, sizeof(char), length, fp);
        if (write_length < length) {
            printf("\t File: %s Write Failed!\n", file_name);
            break;
        } else {
            received_length += length;
            if (received_length == update_file_size) {
                break;
            }
        }
        bzero(buffer, BUFFER_SIZE);
    }

    printf("3.Receive File: %s From Server[%s] Finished!\n", file_name, upgrade_client_ip);
    // 传输完毕，关闭fp
    fclose(fp);

    if (check_camera_update_file()) {
        tar_xzvf_local_update_file();
        if (local_update_file_sha1sum_check() == true) {
            bzero(buffer, sizeof(buffer));
            sprintf(buffer, "%s", SHA1SUM_OKAY_FLAG);

            char old_version_path[128] = {0};
            sprintf(old_version_path, "%s", LOCAL_UPDATE_DIR);
            if (access(old_version_path, F_OK) == 0) {
                remove_dir(old_version_path);
            }
            copy_upgrade_file();
        } else {
            bzero(buffer, sizeof(buffer));
            sprintf(buffer, "%s", SHA1SUM_FAIL_FLAG);
        }
        send(client_socket, buffer, BUFFER_SIZE, 0);
    }

    // 传输完毕，关闭socket
    close(client_socket);
    // ret = 1;
    return /* (void *)&ret */NULL;
}

static ak_pthread_t upgrade_client_thread_id;
static bool upgrade_client_thread_run = false;

bool create_upgrade_client_task(char *server_ip) {
    if(upgrade_client_thread_run){
        return false;
    }
    upgrade_client_thread_run = true;
    ak_thread_create(&upgrade_client_thread_id, connect_server_task, (void*)server_ip, ANYKA_THREAD_NORMAL_STACK_SIZE, -1);
    return true;
}

bool destroy_upgrade_client_task(void) {
    if (upgrade_client_thread_run == false) {
        return false;
    }
    upgrade_client_thread_run = false;
    ak_thread_join(upgrade_client_thread_id);
    upgrade_client_thread_id = 0;
    return true;
}