#pragma once

#if defined(STM32F0)
    #include "stm32f0xx_hal.h"
#elif defined(STM32F1)
    #include "stm32f1xx_hal.h"
#elif defined(STM32F2)
    #include "stm32f2xx_hal.h"
#elif defined(STM32F3)
    #include "stm32f3xx_hal.h"
#elif defined(STM32F4)
    #include "stm32f4xx_hal.h"
#elif defined(STM32F7)
    #include "stm32f7xx_hal.h"
#elif defined(STM32G0)
    #include "stm32g0xx_hal.h"
#elif defined(STM32G4)
    #include "stm32g4xx_hal.h"
#elif defined(STM32H5)
    #include "stm32h5xx_hal.h"
#elif defined(STM32H7)
    #include "stm32h7xx_hal.h"
#elif defined(STM32L0)
    #include "stm32l0xx_hal.h"
#elif defined(STM32L1)
    #include "stm32l1xx_hal.h"
#elif defined(STM32L4)
    #include "stm32l4xx_hal.h"
#elif defined(STM32L5)
    #include "stm32l5xx_hal.h"
#elif defined(STM32U0)
    #include "stm32u0xx_hal.h"
#elif defined(STM32U5)
    #include "stm32u5xx_hal.h"
#elif defined(STM32WB)
    #include "stm32wbxx_hal.h"
#elif defined(STM32WL)
    #include "stm32wlxx_hal.h"
#else
    #error "Unsupported STM32 family"
#endif

#include <cstdint>
#include <limits>
#include <utility>

namespace vtx
{
    namespace ina226
    {

        // Fixed INA226 manufacturer id stored in register 0xFE
        constexpr std::uint32_t default_manufacturer_id = 0x5449u;

        // All available 16 Bit R/W data registers
        enum class data_register : std::uint8_t
        {
            configuration = 0x00,
            shunt_voltage = 0x01,
            bus_voltage = 0x02,
            power = 0x03,
            current = 0x04,
            calibration = 0x05,
            mask_enable = 0x06,
            alert_limit = 0x07,
            manufacturer_id = 0xFE,
            die_id = 0xFF,
        };

        // Number of samples to average
        enum class averaging : std::uint8_t
        {
            average_1 = 0b000,
            average_4 = 0b001,
            average_16 = 0b010,
            average_64 = 0b011,
            average_128 = 0b100,
            average_256 = 0b101,
            average_512 = 0b110,
            average_1024 = 0b111
        };

        // Shunt voltage conversion time
        enum class shunt_voltage_conversion_time : std::uint8_t
        {
            conv_140us = 0b000,
            conv_204us = 0b001,
            conv_332us = 0b010,
            conv_588us = 0b011,
            conv_1100us = 0b100,
            conv_2116us = 0b101,
            conv_4156us = 0b110
        };

        // Bus voltage conversion time
        enum class bus_voltage_conversion_time : std::uint8_t
        {
            conv_140us = 0b000,
            conv_204us = 0b001,
            conv_332us = 0b010,
            conv_588us = 0b011,
            conv_1100us = 0b100,
            conv_2116us = 0b101,
            conv_4156us = 0b110,
            conv_8244us = 0b111
        };

        // Device power/operation modes
        enum class operation_mode : std::uint8_t
        {
            power_down = 0b000,
            shunt_triggered = 0b001,
            bus_triggered = 0b010,
            shunt_bus_triggered = 0b011,
            shunt_continuous = 0b101,
            bus_continuous = 0b110,
            shunt_bus_continuous = 0b111
        };

        // Alert pin related flags
        enum class mask_enable : std::uint16_t
        {
            shunt_voltage_overvoltage = 1u << 15,
            shunt_voltage_undervoltage = 1u << 14,
            bus_voltage_overvoltage = 1u << 13,
            bus_voltage_undervoltage = 1u << 12,
            power_over_limit = 1u << 11,
            conversion_ready = 1u << 10,
            alert_polarity_inverted = 1u << 1,
            alert_latch_enabled = 1u << 0,
        };

        /**
         * @brief Combines two INA226 mask/enable flags.
         *
         * This operator allows multiple @ref mask_enable values to be combined
         * using the bitwise OR operator.
         *
         * @param lhs Left-hand mask flag.
         * @param rhs Right-hand mask flag.
         * @return Combined mask flags.
         */
        [[nodiscard]]
        constexpr mask_enable operator|(mask_enable const lhs, mask_enable const rhs)
        {
            return static_cast<mask_enable>(
                static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
        }

        // Available connections for the address pins A0 and A1
        enum class address_pin : std::uint8_t
        {
            gnd,
            vs,
            sda,
            scl
        };

        /**
         * @brief Gets the INA226 I2C address from the A0 and A1 address pin states.
         *
         * @param a0 The state of the A0 pin. GND = false, VS = true
         * @param a1 The state of the A1 pin. GND = false, VS = true
         * @return The resulting I2C address.
         */
        [[nodiscard]]
        constexpr std::uint8_t address_from_pins(
            address_pin const a0,
            address_pin const a1)
        {
            constexpr std::uint8_t addresses[4][4] = {
                {0b100'0000, 0b100'0010, 0b100'0001, 0b100'0011},
                {0b100'0100, 0b100'0110, 0b100'0101, 0b100'0111},
                {0b100'0001, 0b100'0011, 0b100'0000, 0b100'0010},
                {0b100'0101, 0b100'0111, 0b100'0100, 0b100'0110},
            };

            return addresses[static_cast<std::uint8_t>(a0)][static_cast<std::uint8_t>(a1)];
        }

        template <I2C_HandleTypeDef* Hi2c, std::uint8_t DeviceAddress>
        class INA226
        {
        public:
            INA226() = default;
            ~INA226() = default;

            /**
             * @brief Checks if the INA226 is available via I2C.
             * @details Checks availability by reading the fixed value manufacturer id register 0xFE
             *
             * @param timeout Maximum time in ms to wait for the INAs response (optional, default 100ms)
             *
             * @retval true Read successful, INA226 available
             * @retval false Read unsuccessful, INA226 not reachable
             */
            [[nodiscard]]
            bool is_available(std::uint32_t const timeout = 100u)
            {
                return read(data_register::manufacturer_id, timeout) == default_manufacturer_id;
            }

            /**
             * @brief Configures the shunt resistor value and current measurement range.
             *
             * Calculates the INA226 calibration register value based on the provided
             * shunt resistance and expected current range. This also updates the internal
             * current and power resolution used by @ref get_current and @ref get_power.
             *
             * @param resistance Shunt resistor value in ohms.
             * @param current_range Expected maximum measurable current in amperes.
             *
             * @retval true Calibration value was valid and written to the device.
             * @retval false Invalid input values or calibration value out of range.
             */
            constexpr bool set_shunt_resistor_range(float const resistance, float const current_range)
            {
                if (current_range <= 0.f || resistance <= 0.f)
                {
                    return false;
                }

                m_current_resolution = current_range / 32768.f;
                m_power_resolution = 25.f * m_current_resolution;

                auto const cal = 0.00512f / (m_current_resolution * resistance);

                if (cal > std::numeric_limits<std::uint16_t>::max())
                {
                    return false;
                }

                write(data_register::calibration, static_cast<std::uint16_t>(cal));
                return true;
            }

            /**
             * @brief Resets the INA226 device.
             *
             * Writes the reset bit to the INA226 configuration register.
             * After reset, the device returns to its default register values.
             */
            static void reset()
            {
                constexpr std::uint16_t ina_reset{0x8000};
                write(data_register::configuration, ina_reset);
            }

            /**
             * @brief Sets the INA226 averaging mode.
             *
             * Configures the number of samples averaged for each measurement.
             *
             * @param averaging_mode Averaging mode to configure.
             *
             * @see averaging
             */
            static void set_averaging_mode(averaging const averaging_mode)
            {
                auto const config_register = read(data_register::configuration);

                write(
                    data_register::configuration,
                    set_bits(config_register, static_cast<std::uint8_t>(averaging_mode), 9));
            }

            /**
             * @brief Sets the bus voltage conversion time.
             *
             * Configures how long the INA226 spends converting each bus voltage sample.
             *
             * @param time Bus voltage conversion time.
             *
             * @see bus_voltage_conversion_time
             */
            static void set_bus_voltage_conversion_time(bus_voltage_conversion_time const time)
            {
                auto const config_register = read(data_register::configuration);

                write(
                    data_register::configuration,
                    set_bits(config_register, static_cast<std::uint8_t>(time), 6));
            }

            /**
             * @brief Sets the shunt voltage conversion time.
             *
             * Configures how long the INA226 spends converting each shunt voltage sample.
             *
             * @param time Shunt voltage conversion time.
             *
             * @see shunt_voltage_conversion_time
             */
            static void set_shunt_voltage_conversion_time(shunt_voltage_conversion_time const time)
            {
                auto const config_register = read(data_register::configuration);

                write(
                    data_register::configuration,
                    set_bits(config_register, static_cast<std::uint8_t>(time), 3));
            }

            /**
             * @brief Sets the INA226 operating mode.
             *
             * Selects whether the INA226 operates in power-down, triggered or continuous
             * measurement mode.
             *
             * @param mode Operation mode to configure.
             *
             * @see operation_mode
             */
            static void set_operation_mode(operation_mode const mode)
            {
                auto const config_register = read(data_register::configuration);

                write(
                    data_register::configuration,
                    set_bits(config_register, static_cast<std::uint8_t>(mode), 0));
            }

            /**
             * @brief Reads the measured shunt voltage.
             *
             * Reads the INA226 shunt voltage register and converts the raw value into volts.
             *
             * @return Shunt voltage [V].
             *
             * @details
             * The INA226 shunt voltage resolution is 2.5 uV/Bit.
             */
            [[nodiscard]]
            static float get_shunt_voltage()
            {
                constexpr float shunt_voltage_resolution{2.5e-6f};
                return read_scaled(data_register::shunt_voltage, shunt_voltage_resolution);
            }

            /**
             * @brief Reads the measured bus voltage.
             *
             * Reads the INA226 bus voltage register and converts the raw value into volts.
             *
             * @return Bus voltage [V].
             *
             * @details
             * The INA226 bus voltage resolution is 1.25 mV/Bit.
             */
            [[nodiscard]]
            static float get_bus_voltage()
            {
                constexpr float bus_voltage_resolution{1.25e-3f};
                return read_signed_scaled(data_register::bus_voltage, bus_voltage_resolution);
            }

            /**
             * @brief Reads the measured current.
             *
             * Reads the INA226 current register and scales the raw value using the current
             * resolution configured by @ref set_shunt_resistor_range.
             *
             * @return Current [A].
             *
             * @note The device must be calibrated using @ref set_shunt_resistor_range before this value is meaningful.
             */
            [[nodiscard]]
            float get_current() const
            {
                return read_signed_scaled(data_register::current, m_current_resolution);
            }

            /**
             * @brief Reads the measured power.
             *
             * Reads the INA226 power register and scales the raw value using the power
             * resolution configured by @ref set_shunt_resistor_range.
             *
             * @return Power [W].
             *
             * @note The device must be calibrated using @ref set_shunt_resistor_range before this value is meaningful.
             */
            [[nodiscard]]
            float get_power() const
            {
                return read_signed_scaled(data_register::power, m_power_resolution);
            }

            /**
             * @brief Reads the INA226 manufacturer ID register.
             *
             * @return Raw 16-bit manufacturer ID value.
             */
            [[nodiscard]]
            static std::uint16_t get_manufacturer_id()
            {
                return read(data_register::manufacturer_id);
            }

            /**
             * @brief Reads the INA226 die ID register.
             *
             * @return Raw 16-bit die ID value.
             */
            [[nodiscard]]
            static std::uint16_t get_die_id()
            {
                return read(data_register::die_id);
            }

            /**
             * @brief Configures the INA226 mask/enable register.
             *
             * Writes alert configuration bits to the INA226 mask/enable register.
             *
             * @param bits Mask/enable flags to write.
             *
             * @see mask_enable
             */
            static void set_mask_enable_bits(mask_enable const bits)
            {
                write(
                    data_register::mask_enable,
                    std::to_underlying(bits));
            }

            /**
             * @brief Reads the active INA226 alert status bits.
             *
             * Reads the mask/enable register and extracts the alert status bits.
             *
             * @return Alert status bits.
             */
            [[nodiscard]]
            static std::uint8_t get_alert_bits()
            {
                return static_cast<std::uint8_t>((read(data_register::mask_enable) >> 2) & 0b111);
            }

            /**
             * @brief Sets the INA226 alert limit register.
             *
             * The interpretation of the limit value depends on the selected alert function
             * in the mask/enable register.
             *
             * @param limit Raw 16-bit alert limit value.
             */
            static void set_alert_limit(std::uint16_t const limit)
            {
                write(data_register::alert_limit, limit);
            }

        private:
            /**
             * @brief Reads a 16-bit INA226 register.
             *
             * Performs an I2C memory read using the STM32 HAL API.
             *
             * @param reg INA226 register to read.
             * @return Raw 16-bit register value.
             */
            [[nodiscard]]
            static std::uint16_t read(data_register const reg, std::uint32_t const timeout = HAL_MAX_DELAY)
            {
                std::uint8_t data[2]{};

                HAL_I2C_Mem_Read(
                    Hi2c,
                    DeviceAddress,
                    static_cast<std::uint8_t>(reg),
                    I2C_MEMADD_SIZE_8BIT,
                    data,
                    2,
                    timeout);

                return (static_cast<std::uint16_t>(data[0]) << 8) | data[1];
            }

            /**
             * @brief Reads a 16-bit INA226 register as a signed value.
             *
             * @param reg INA226 register to read.
             * @return Raw register value interpreted as signed 16-bit integer.
             */
            [[nodiscard]]
            static std::int16_t read_signed(data_register const reg)
            {
                return static_cast<std::int16_t>(read(reg));
            }

            /**
             * @brief Reads an unsigned register value and applies a scale factor.
             *
             * @param reg INA226 register to read.
             * @param scaling Scale factor applied to the raw register value.
             *
             * @return Scaled floating-point value.
             */
            [[nodiscard]]
            static float read_scaled(data_register const reg, float const scaling)
            {
                return static_cast<float>(read(reg)) * scaling;
            }

            /**
             * @brief Reads a signed register value and applies a scale factor.
             *
             * @param reg INA226 register to read.
             * @param scaling Scale factor applied to the signed raw register value.
             *
             * @return Scaled floating-point value.
             */
            [[nodiscard]]
            static float read_signed_scaled(data_register const reg, float const scaling)
            {
                return static_cast<float>(read_signed(reg)) * scaling;
            }

            /**
             * @brief Writes a 16-bit value to an INA226 register.
             *
             * Performs an I2C memory write using the STM32 HAL API.
             *
             * @param reg INA226 register to write.
             * @param value Raw 16-bit value to write.
             *
             * @return HAL status returned by HAL_I2C_Mem_Write.
             */
            static auto write(data_register const reg, std::uint16_t const value)
            {
                std::uint8_t data[] = {
                    static_cast<std::uint8_t>(value >> 8),
                    static_cast<std::uint8_t>(value)};

                return HAL_I2C_Mem_Write(
                    Hi2c,
                    DeviceAddress << 1,
                    static_cast<std::uint8_t>(reg),
                    I2C_MEMADD_SIZE_8BIT,
                    data,
                    2,
                    HAL_MAX_DELAY);
            }

            /**
             * @brief Replaces a bit field inside a 16-bit register value.
             *
             * Clears a bit field at the specified offset and inserts the provided value.
             *
             * @param reg_value Original 16-bit register value.
             * @param value New bit-field value.
             * @param offset Bit offset of the field.
             * @param length Length of the field in bits.
             *
             * @return Modified 16-bit register value.
             */
            static constexpr auto set_bits(
                std::uint16_t const reg_value,
                std::uint8_t const value,
                std::uint8_t const offset,
                std::uint8_t const length = 3)
            {
                auto const mask = static_cast<std::uint16_t>(
                    ((std::uint16_t{1} << length) - 1u) << offset);

                return static_cast<std::uint16_t>(
                    (reg_value & ~mask) | ((static_cast<std::uint16_t>(value) << offset) & mask));
            }

            float m_current_resolution{};
            float m_power_resolution{};
        };
    }
}