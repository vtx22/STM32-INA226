#include "I2C.hpp"

I2C::I2C(I2C_HandleTypeDef* hi2c) : _hi2c(hi2c)
{

}

I2C_Result_t I2C::writeByte(uint8_t device_address, uint8_t register_address, uint8_t data)
{
	uint8_t d[2];

		/* Format array to send */
		d[0] = register_address;
		d[1] = data;

		/* Try to transmit via I2C */
		if (HAL_I2C_Master_Transmit(_hi2c, (device_address << 1), (uint8_t *)d, 2, 1000) != HAL_OK) {
			/* Check error */
			if (HAL_I2C_GetError(_hi2c) != HAL_I2C_ERROR_AF) {

			}

			/* Return error */
			return I2C_Result_Error;
		}

		/* Return OK */
		return I2C_Result_Ok;
}

I2C_Result_t I2C::writeByte(uint8_t device_address, uint8_t data)
{
	uint8_t d[1];

		/* Format array to send */

		d[0] = data;

		/* Try to transmit via I2C */
		if (HAL_I2C_Master_Transmit(_hi2c, (device_address << 1), (uint8_t *)d, 2, 1000) != HAL_OK) {
			/* Check error */
			if (HAL_I2C_GetError(_hi2c) != HAL_I2C_ERROR_AF) {

			}

			/* Return error */
			return I2C_Result_Error;
		}

		/* Return OK */
		return I2C_Result_Ok;
}

I2C_Result_t I2C::write2Bytes(uint8_t device_address, uint8_t register_address, uint16_t data)
{
	uint8_t d[3];

		/* Format array to send */
		d[0] = register_address;
		d[1] = (uint8_t)(data >> 8);
		d[2] = (uint8_t)(data & 255);

		/* Try to transmit via I2C */
		if (HAL_I2C_Master_Transmit(_hi2c, (device_address << 1), (uint8_t *)d, 3, 1000) != HAL_OK) {
			/* Check error */
			if (HAL_I2C_GetError(_hi2c) != HAL_I2C_ERROR_AF) {

			}

			/* Return error */
			return I2C_Result_Error;
		}

		/* Return OK */
		return I2C_Result_Ok;
}

I2C_Result_t I2C::readMultiBytes(uint8_t device_address, uint8_t register_address, uint8_t* data, uint16_t count)
{
	//if (HAL_I2C_Master_Transmit(hi2c, (uint8_t)device_address, &register_address, 1, 1000) != HAL_OK) {
	//device_address = 0x83;
	if (HAL_I2C_Master_Transmit(_hi2c, (device_address << 1) , &register_address, 1, 1000) != HAL_OK) {
		/* Check error */
		if (HAL_I2C_GetError(_hi2c) != HAL_I2C_ERROR_AF) {

		}

		/* Return error */
		return I2C_Result_Error;
	}

	/* Receive multiple byte */
	if (HAL_I2C_Master_Receive(_hi2c, (device_address << 1) + 1, data, count, 1000) != HAL_OK) {
		/* Check error */
		if (HAL_I2C_GetError(_hi2c) != HAL_I2C_ERROR_AF) {

		}

		/* Return error */
		return I2C_Result_Error;
	}

	/* Return OK */
	return I2C_Result_Ok;
}

uint8_t I2C::readOneByte(uint8_t device_address, uint8_t register_address)
{
	uint8_t data[1];
	if(readMultiBytes(device_address, register_address, data, 1) == I2C_Result_Error)
	{
		return 0;
	}
	return data[0];
}
