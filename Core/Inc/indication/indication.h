/*
 * indication.h
 *
 *  Created on: May 27, 2024
 *      Author: lepikhov
 */

#ifndef INC_INDICATION_INDICATION_H_
#define INC_INDICATION_INDICATION_H_


#define LEDS_NUMBER 32
#define DIGITS_NUMBER 4

#define MAX_CONNECT_TRY_COUNT 2

enum INDICATION_STATES {
	INDIICATION_STATE_IDLE = 0,
	INDICATION_STATE_LEDS1,
	INDICATION_STATE_LEDS2,
	INDICATION_STATE_LEDS3,
	INDICATION_STATE_LEDS4,
	INDICATION_STATE_DIGIT1,
	INDICATION_STATE_DIGIT2,
	INDICATION_STATE_DIGIT3,
};

enum INDICATION_TYPES {
	INDICATION_TYPE_TM1650 = 0,
	INDICATION_TYPE_IS31FL3731
};

struct indication_t {
	enum INDICATION_TYPES type;
	uint8_t leds[DIGITS_NUMBER];
	enum INDICATION_STATES state;
	uint8_t connect_try_count;
};

struct indication_rxtx_t {
	uint8_t rx_counter;
	uint8_t tx_counter;
	bool rx_flag;
	bool tx_flag;
};

void indication_init(void);
void indication_func(void);
void indication_func_TM1650(void);
void indication_func_IS31FL3731(void);

void indication_set_leds(uint8_t addr, uint8_t mean);
uint8_t indication_get_leds(uint8_t addr);

void indication_set_led(uint8_t addr);
void indication_clear_led(uint8_t addr);


#define COMMUNICATION_RX_LED 26
#define COMMUNICATION_TX_LED 27

#define DEVICE_HEALTH_LED 24

#define RX_TX_LED_COUNTER_LOADER 40

void indication_start_rx_led(void);
void indication_start_tx_led(void);
void indication_rx_tx_led_func(void);
void indication_health_led_func(bool set);



#endif /* INC_INDICATION_INDICATION_H_ */
