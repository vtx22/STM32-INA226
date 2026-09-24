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
    enum class ina226_register : std::uint8_t
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

    enum class ina226_averaging : std::uint8_t
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

    enum class ina226_shunt_voltage_conversion_time : std::uint8_t
    {
        conv_140us = 0b000,
        conv_204us = 0b001,
        conv_332us = 0b010,
        conv_588us = 0b011,
        conv_1100us = 0b100,
        conv_2116us = 0b101,
        conv_4156us = 0b110
    };

    enum class ina226_bus_voltage_conversion_time : std::uint8_t
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

    enum class ina226_operation_mode : std::uint8_t
    {
        power_down = 0b000,
        shunt_triggered = 0b001,
        bus_triggered = 0b010,
        shunt_bus_triggered = 0b011,
        shunt_continuous = 0b101,
        bus_continuous = 0b110,
        shunt_bus_continuous = 0b111
    };

    enum class ina226_mask_enable : std::uint16_t
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

    constexpr ina226_mask_enable operator|(ina226_mask_enable const lhs, ina226_mask_enable const rhs)
    {
        return static_cast<ina226_mask_enable>(
            static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
    }

    template <I2C_HandleTypeDef* Hi2c, std::uint8_t DeviceAddress>
    class INA226
    {
    public:
        INA226(float const shunt_resistance, float const current_range)
        {
            set_shunt_resistor_range(shunt_resistance, current_range);
        }

        bool set_shunt_resistor_range(float const resistance, float const current_range)
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

            write(ina226_register::calibration, static_cast<std::uint16_t>(cal));
            return true;
        }

        static void reset()
        {
            constexpr std::uint16_t ina_reset{0x8000};
            write(ina226_register::configuration, ina_reset);
        }

        static void set_averaging_mode(ina226_averaging const averaging_mode)
        {
            auto const config_register = read(ina226_register::configuration);

            write(
                ina226_register::configuration,
                set_bits(config_register, static_cast<std::uint8_t>(averaging_mode), 9));
        }

        static void set_bus_voltage_conversion_time(ina226_bus_voltage_conversion_time const time)
        {
            auto const config_register = read(ina226_register::configuration);

            write(
                ina226_register::configuration,
                set_bits(config_register, static_cast<std::uint8_t>(time), 6));
        }

        static void set_shunt_voltage_conversion_time(ina226_shunt_voltage_conversion_time const time)
        {
            auto const config_register = read(ina226_register::configuration);

            write(
                ina226_register::configuration,
                set_bits(config_register, static_cast<std::uint8_t>(time), 3));
        }

        static void set_operation_mode(ina226_operation_mode const mode)
        {
            auto const config_register = read(ina226_register::configuration);

            write(
                ina226_register::configuration,
                set_bits(config_register, static_cast<std::uint8_t>(mode), 0));
        }

        [[nodiscard]]
        static float get_shunt_voltage()
        {
            constexpr float shunt_voltage_resolution{2.5e-6f};
            return read_scaled(ina226_register::shunt_voltage, shunt_voltage_resolution);
        }

        [[nodiscard]]
        static float get_bus_voltage()
        {
            constexpr float bus_voltage_resolution{1.25e-3f};
            return read_signed_scaled(ina226_register::bus_voltage, bus_voltage_resolution);
        }

        [[nodiscard]]
        float get_current() const
        {
            return read_signed_scaled(ina226_register::current, m_current_resolution);
        }

        [[nodiscard]]
        float get_power() const
        {
            return read_signed_scaled(ina226_register::power, m_power_resolution);
        }

        [[nodiscard]]
        static std::uint16_t get_manufacturer_id()
        {
            return read(ina226_register::manufacturer_id);
        }

        [[nodiscard]]
        static std::uint16_t get_die_id()
        {
            return read(ina226_register::die_id);
        }

        static void set_mask_enable_bits(ina226_mask_enable const bits)
        {
            write(
                ina226_register::mask_enable,
                std::to_underlying(bits));
        }

        [[nodiscard]]
        static std::uint8_t get_alert_bits()
        {
            return static_cast<std::uint8_t>((read(ina226_register::mask_enable) >> 2) & 0b111);
        }

        static void set_alert_limit(std::uint16_t const limit)
        {
            write(ina226_register::alert_limit, limit);
        }

    private:
        [[nodiscard]]
        static std::uint16_t read(ina226_register const reg)
        {
            std::uint8_t data[2]{};

            HAL_I2C_Mem_Read(
                Hi2c,
                DeviceAddress,
                static_cast<std::uint8_t>(reg),
                I2C_MEMADD_SIZE_8BIT,
                data,
                2,
                HAL_MAX_DELAY);

            return (static_cast<std::uint16_t>(data[0]) << 8) | data[1];
        }

        [[nodiscard]]
        static std::int16_t read_signed(ina226_register const reg)
        {
            return static_cast<std::int16_t>(read(reg));
        }

        [[nodiscard]]
        static float read_scaled(ina226_register const reg, float const scaling)
        {
            return static_cast<float>(read(reg)) * scaling;
        }

        [[nodiscard]]
        static float read_signed_scaled(ina226_register const reg, float const scaling)
        {
            return static_cast<float>(read_signed(reg)) * scaling;
        }

        static auto write(ina226_register const reg, std::uint16_t const value)
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
