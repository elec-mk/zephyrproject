#ifndef _HT16K33_H
#define _HT16K33_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>

class HT16K33
{
private:
    const struct device *_i2c_dev;

    uint8_t _tx_buffer[2];
    uint8_t _rx_buffer[2];

    const uint8_t _address = 0x70;
    const uint32_t MAX_ATTEMPTS = 10;

    void _i2c_write(uint32_t bytes, uint32_t attempts);


public:
    HT16K33(const struct device *i2c);
    ~HT16K33();

    void initialise();

    void clear_display();
    void set_brightness(int brightness);
    void set_system_setup(bool oscillator_enable);
    void set_display_setup(bool display_enable, uint8_t blink_rate);

    void set_matrix_row(uint8_t collumn, uint8_t row);
};

#endif