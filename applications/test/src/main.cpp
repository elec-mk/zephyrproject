/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// zephyr drivers etc
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>

// project files
#include "HT16K33.h"

#define LOG_MODULE_NAME main
LOG_MODULE_REGISTER(LOG_MODULE_NAME, LOG_LEVEL_DBG);

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   250

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
           
/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

    
void i2c_scan(bool print, const struct device *i2c_dev)
{
    if(print)
    {
        printk("\nI2C Bus Scan\n");
        printk("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    }

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0 && print) {
            printk("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        ret = i2c_read(i2c_dev, &rxdata, 1, addr);
        if ( print )
        {
            printk(ret < 0 ? "." : "@");
            printk(addr % 16 == 15 ? "\n" : "  ");
        }
    }	
}


int main(void)
{
	int ret;
	const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));
	if(!device_is_ready(i2c_dev))
	{
		LOG_ERR("I2C device not ready!");
		return 0;
	}

	i2c_scan(true, i2c_dev);

	HT16K33 matrix(i2c_dev);

	k_msleep(1000);
	
	
	printk("Hello World! from %s\n", CONFIG_BOARD);
    
    if (!device_is_ready(led.port)) 
	{
		LOG_ERR("LED port not ready");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) 
	{
		LOG_ERR("Failed to configure led");
		return 0;
	}

	int com = 0x00;
	int row = 0x01;
	int brightness = 0;
	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) 
		{
			LOG_ERR("Failed to toggle led");
			return 0;
		}
		
		matrix.set_matrix_row(com, row);

		matrix.set_brightness(brightness++);
			
		if(row == 0x80)
		{
			row = 0x01;
			com++;
		}
		else
		{
			row = row << 1;
		}

		if(com == 0x08)
		{
			row = 0x01;
			com = 0x00;
		}

		if(brightness == 0x07)
		{
			brightness = 0;
		}

		k_msleep(SLEEP_TIME_MS);
	}
}


// state machine
// 1 - discharged - cacitor banch fully discharged
//  -> 2 - charging - boost stage enabled, capacitor bank charging up
//  <- 3 - discharging -