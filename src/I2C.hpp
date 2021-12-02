//Library for using the I2C-Bus
#pragma once
#include <stdint.h>
#include "stm32f1xx_hal.h"

typedef enum _I2C_Result_t {
    I2C_Result_Ok = 0x00,
    I2C_Result_Error,
} I2C_Result_t;

class I2C
{
	public:
		I2C(I2C_HandleTypeDef* hi2c);

		I2C_Result_t writeByte(uint8_t device_address, uint8_t register_address, uint8_t data);
		I2C_Result_t writeByte(uint8_t device_address, uint8_t data);
		I2C_Result_t write2Bytes(uint8_t device_address, uint8_t register_address, uint16_t data);

		I2C_Result_t readMultiBytes(uint8_t device_address, uint8_t register_address, uint8_t* data, uint16_t count);
		uint8_t readOneByte(uint8_t device_address, uint8_t register_address);


	private:
		I2C_HandleTypeDef* _hi2c;

};


