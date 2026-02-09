#include "rgbled.h"
#include "driver/gpio.h"
#include "debug_cf.h"

#define RED_LED 20
#define GREEN_LED 19
#define BLUE_LED 18

void rgbled_init()
{
	gpio_config_t redLedConfig;
	redLedConfig.pin_bit_mask = 1 << RED_LED;
	redLedConfig.pull_down_en = 0;
	redLedConfig.pull_up_en = 0;
	redLedConfig.mode = GPIO_MODE_OUTPUT;

	gpio_config_t greenLedConfig;
	greenLedConfig.pin_bit_mask = 1 << GREEN_LED;
	greenLedConfig.pull_down_en = 0;
	greenLedConfig.pull_up_en = 0;
	greenLedConfig.mode = GPIO_MODE_OUTPUT;

	gpio_config_t blueLedConfig;
	blueLedConfig.pin_bit_mask = 1 << BLUE_LED;
	blueLedConfig.pull_down_en = 0;
	blueLedConfig.pull_up_en = 0;
	blueLedConfig.mode = GPIO_MODE_OUTPUT;

	gpio_config(&redLedConfig);
	gpio_config(&blueLedConfig);
	gpio_config(&greenLedConfig);
}

void process_rgb_command(uint8_t *packet, uint8_t size)
{
	DEBUG_PRINT_LOCAL("Processing RGB packet");
	DEBUG_PRINT_LOCAL("Size = %d",size);

	if (packet[0] != 'R' || packet[1] != 'G' ||
		packet[2] != 'B' || size < 6)
	{
		// not a valid rgb packet
		return;
	}


	_Bool red = packet[3] > 0;
	_Bool green = packet[4] > 0;
	_Bool blue = packet[5] > 0;

	set_rgbled(red,green,blue);
}

void set_rgbled(_Bool r, _Bool g, _Bool b)
{
	gpio_set_level(RED_LED,r);
	gpio_set_level(GREEN_LED,g);
	gpio_set_level(BLUE_LED,b);
}
