#include "INA226.hpp"

INA226::INA226(I2C_HandleTypeDef* hi2c) : _hi2c(hi2c)
{
	_i2c = new I2C(_hi2c);
	updateConfiguration();
}

void INA226::updateConfiguration()
{
	uint16_t reg = 0x00 + (_reset << 15);

	reg |= (_average << 9);
	reg |= (_convTime << 6);
	reg |= (_convTime << 3);
	reg |= _mode;

	_i2c->write2Bytes(_address, CONFIG_REG, reg);

	calibrateDevice();
}

float INA226::getShuntVol()
{
	uint8_t data[2] = {0x00, 0x00};
	_i2c->readMultiBytes(_address, SHUNTVOL_REG, data, 2);

	return (float)((int16_t)((data[0] << 8) + data[1]) * _voltageResolutionShunt);
}

float INA226::getBusVol()
{
	uint8_t data[2] = {0x00, 0x00};
	_i2c->readMultiBytes(_address, BUSVOL_REG, data, 2);

	return (float)(((data[0] << 8) + data[1]) * _voltageResolutionBus);
}

float INA226::getPower()
{
	uint8_t data[2] = {0x00, 0x00};
	_i2c->readMultiBytes(_address, POWER_REG, data, 2);

	return (float)(((data[0] << 8) + data[1]) * 25 * _currentResolution);
}

float INA226::getCurrent()
{
	uint8_t data[2] = {0x00, 0x00};
	_i2c->readMultiBytes(_address, CURRENT_REG, data, 2);

	return (float)(((data[0] << 8) + data[1]) * _currentResolution);
}

void INA226::calibrateDevice()
{
	//Calibration for Current Measurement
	//resolution = maxCurrent / pow(2, 15) =approx 500uA/Bit;

	//_cal = (uint16_t)(0.00512 / (_currentResolution * _shuntResistance));

	_i2c->write2Bytes(_address, CALIB_REG, _cal);

}


