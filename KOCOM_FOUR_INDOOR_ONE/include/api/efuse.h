//
// Created by michael on 22-4-21.
//

#ifndef UUID_EFUSE_H
#define UUID_EFUSE_H
/********************** IOCTL *********************************************/
#define AK_EFUSE_IOC_MAGIC			'F'

#define DO_GLOBE_ID_GET_LEN		_IOR(AK_EFUSE_IOC_MAGIC, 0x101, int)
#define DO_GLOBE_ID_GET			_IOR(AK_EFUSE_IOC_MAGIC, 0x102, int)
#define DO_CUSTOMER_ID_GET_LEN	_IOR(AK_EFUSE_IOC_MAGIC, 0x103, int)
#define DO_CUSTOMER_ID_GET		_IOR(AK_EFUSE_IOC_MAGIC, 0x104, int)
#define DO_CUSTOMER_ID_SET		_IOW(AK_EFUSE_IOC_MAGIC, 0x105, int)

#define ERR_PARAM         1  // ioctl参数检查错误
#define ERR_NOLOCK        2  // GLOBE ID读取，没有锁定错误码
#define ERR_LOCKED        3  // Customer ID写入，已经锁定错误码
#define ERR_OTHER         4  // 其他错误，数据比对错误等

int get_global_id(unsigned char *uuid,int *uuid_length);

#endif //UUID_EFUSE_H
