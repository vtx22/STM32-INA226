#pragma once
#include <stdint.h>
#include <math.h>
#include "stm32f1xx_hal.h"
#include "I2C.hpp"

//All Register Addresses of the INA226, each Register 16 Bits in length
const uint8_t CONFIG_REG 	= 0x00;		//Config Register
const uint8_t SHUNTVOL_REG 	= 0x01;		//Shunt Voltage Register
const uint8_t BUSVOL_REG 	= 0x02;		//Bus Voltage Register
const uint8_t POWER_REG 	= 0x03;		//Power Register
const uint8_t CURRENT_REG 	= 0x04;		//Current Register
const uint8_t CALIB_REG 	= 0x05;		//Calibration Register
const uint8_t MASKEN_REG 	= 0x06;		//Mask / Enable Register
const uint8_t ALERT_REG 	= 0x07;		//Alert Register
const uint8_t MANUF_REG 	= 0xFE;		//Manufacturer ID Register
const uint8_t UNQID_REG 	= 0xFF;		//Unique ID Register

enum INA226_AVERAGES{		//Number of samples to be averaged
    AVERAGE_1       = 0b000,
    AVERAGE_4       = 0b001,
    AVERAGE_16      = 0b010,
    AVERAGE_64      = 0b011,
    AVERAGE_128     = 0b100,
    AVERAGE_256     = 0b101,
    AVERAGE_512     = 0b110,
    AVERAGE_1024    = 0b111
};

enum INA226_CONV_TIME{		//Conversion Time in us
    CONV_TIME_140   = 0b000,
    CONV_TIME_204   = 0b001,
    CONV_TIME_332   = 0b010,
    CONV_TIME_588   = 0b011,
    CONV_TIME_1100  = 0b100,
    CONV_TIME_2116  = 0b101,
    CONV_TIME_4156  = 0b110,
    CONV_TIME_8244  = 0b111
};

enum INA226_MEASURE_MODE{	//Measure Modes
    POWER_DOWN      	= 0b000,
    SHUNT_TRIGGERED     = 0b001,
    BUS_TRIGGERED      	= 0b010,
	SHUNTBUS_TRIGGERED	= 0b011,
	SHUNT_CONTINOUS		= 0b101,
	BUS_CONTINOUS		= 0b110,
	SHUNTBUS_CONTINOUS	= 0b111
};



class INA226
{
	public:
		INA226(I2C_HandleTypeDef* hi2c);

		void updateConfiguration();
		void calibrateDevice();

		uint16_t readConfiguration();


		float getShuntVol();	//Returns the current Shunt Voltage in V
		float getBusVol();		//Returns the current Bus 	Voltage	in V
		float getPower();		//Returns the current Power			in W
		float getCurrent();		//Returns the current Current		in A


	private:

		bool _reset = 0;
		INA226_AVERAGES _average = AVERAGE_4;
		INA226_CONV_TIME _convTime = CONV_TIME_332;
		INA226_MEASURE_MODE _mode = SHUNTBUS_CONTINOUS;


		uint16_t _cal = 1024;
		float _shuntResistance = 0.01;			//Shunt Resistance in Ohms
		float _currentResolution = 0.00005;		//Calculated Current Resolution 	in A/Bit
		float _voltageResolutionBus = 1.25e-3;	//Fixed Bus Voltage Resolution		in V/Bit
		float _voltageResolutionShunt = 2.5e-6;	//Fixed Shunt Voltage Resolution	in V/Bit

		uint8_t _address = 0x41;				//I2C Address of the INA226 Sensor
		I2C_HandleTypeDef* _hi2c;				//Used I2C Bus
		I2C* _i2c;								//I2C Object of the I2C.hpp Lib
};
