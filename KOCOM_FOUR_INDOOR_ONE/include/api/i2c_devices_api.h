#ifndef I2C_DEVICES_H
#define I2C_DEVICES_H
#include "stdbool.h"


typedef enum
{
	I2C_DEVICES_0,
	I2C_DEVICES_1,
	I2C_DEVICES_2,
	I2C_DEVICES_3,
	I2C_DEVICES_4,
	I2C_DEVICES_TOTAL
}I2C_DEVICES;

typedef struct
{
	unsigned char addres;
	unsigned char data;
}i2c_data;


bool i2c_init(I2C_DEVICES devices);

bool i2c_write(I2C_DEVICES devices,unsigned short addres,const i2c_data* data,int len);

bool i2c_read(I2C_DEVICES devices,unsigned short addres,i2c_data* data);


#endif

