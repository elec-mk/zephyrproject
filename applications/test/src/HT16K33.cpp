#include "HT16K33.h"

#define LOG_MODULE_NAME ht16k33
LOG_MODULE_REGISTER(LOG_MODULE_NAME, LOG_LEVEL_DBG);

HT16K33::HT16K33(const struct device *i2c)
{
    _i2c_dev = i2c;

    if(!device_is_ready(_i2c_dev))
	{
		LOG_ERR("I2C device not ready!");
	}

    initialise();
}

HT16K33::~HT16K33()
{
}


void HT16K33::initialise()
{
    set_system_setup(true);
    set_display_setup(true, 0);
    clear_display();
    set_brightness(0);
}

void HT16K33::clear_display()
{
    for (int com = 0; com < 0x08; com++)
	{
		_tx_buffer[0] = com*2;
		_tx_buffer[1] = 0x00;
		_i2c_write(2, 10);
	}
}

void HT16K33::set_brightness(int brightness)
{
    // limit check
    if(brightness > 0x0F) brightness = 0x0F;

    _tx_buffer[0] = 0b11100000 | brightness;
	
    _i2c_write(1, 10);
}

void HT16K33::set_system_setup(bool oscillator_enable)
{
    // register address 
    // register value
	_tx_buffer[0] = 0x20 | (uint8_t)oscillator_enable;
	//_tx_buffer[1] = 0b00100000 | oscillator_enable;
	
    LOG_DBG("Setting SYSTEM_SETUP register to 0x%02X", _tx_buffer[0]);
    _i2c_write(1, 10);
}

void HT16K33::set_display_setup(bool display_enable, uint8_t blink_rate)
{
    // 
	_tx_buffer[0] = 0x80 | (blink_rate << 0x01) | (uint8_t)display_enable;

    LOG_DBG("Setting DISPLAY_SETUP register to 0x%02X", _tx_buffer[0]);

    _i2c_write(1, 10);
}

void HT16K33::set_matrix_row(uint8_t collumn, uint8_t row)
{  
    if(row & 0x01)
    {   
        row = ((row & 0b11111110) >> 1) + 0b10000000;
    }
    else
    {
        row = row  >> 1;
    }


    // register address 
    // register value
	_tx_buffer[0] = collumn * 2;
	_tx_buffer[1] = row;
	
    //LOG_DBG("setting matrix com %u row %u", collumn, row);
    _i2c_write(2, 10);

 
}

void HT16K33::_i2c_write(uint32_t bytes, uint32_t attempts)
{
    while( i2c_write(_i2c_dev, _tx_buffer, 2, _address)!= 0)
    {
        if(attempts >= 10)
        {
            LOG_ERR("HT16K33 I2C write failed after %u attempts", attempts);
            break;
        }
    }
}