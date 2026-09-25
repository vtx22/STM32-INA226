# STM32 INA226 Power Monitor Library

C++ header-only library for interfacing a Texas Instruments INA226 power monitor
with STM32 microcontrollers using the STM32 HAL I2C API.

The INA226 can measure:

- Shunt voltage
- Bus voltage
- Current
- Power
- Alert conditions

## Features

- Header-only C++ interface
- STM32 HAL I2C backend
- Configurable averaging mode
- Configurable bus voltage conversion time
- Configurable shunt voltage conversion time
- Configurable operation mode
- Current and power calibration
- Bus voltage, shunt voltage, current and power readings
- Manufacturer and die ID reading
- Alert mask and alert limit configuration
- A0/A1 pin state to address conversion

## Usage 
### Minimal Usage Example

```C++
#include <INA226.h>

using namespace vtx;

int main()
{
    // Create INA226 object, specify I2C interface and address
    INA226<&hi2c1,
        ina226_address_from_pins(
            ina226_address_pin::gnd,
            ina226_address_pin::gnd)> ina{};
    
    // Check if the INA226 responds via I2C
    if (!ina.is_available())
    {
        // Error, device not reachable!   
    }
    
    // Specify your shunt resistance (10 mOhm here) 
    // and the maximum current you are expecting (5A here)
    ina.set_shunt_resistor_range(10e-3f, 5.f);
    
    while (true)
    {
        // ...
        
        // Read whatever values you need
        auto const bus_voltage = ina.get_bus_voltage();
        auto const current = ina.get_current();
        
        // ...
        
        HAL_Delay(1000);
    }
}
```