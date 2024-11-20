/*
 * IS31FL3731.h
 *
 *  Created on: Nov 18, 2024
 *      Author: lepikhov
 */

#ifndef  INC_INDICATION_IS31FL3731_H_
#define  INC_INDICATION_IS31FL3731_H_

#define ADDR 0x74

#define COMMAND_REGISTER 0xFD

#define BANK_1 0x00
#define BANK_2 0x01
#define BANK_3 0x02
#define BANK_4 0x03
#define BANK_5 0x04
#define BANK_6 0x05
#define BANK_7 0x06
#define BANK_8 0x07
#define BANK_FUNCTIONREG 0x0B

#define MODE_SELECT 0x00
#define FRAME_SELECT 0x01
#define AUTO_PLAY_CONTROL_1 0x02
#define AUTO_PLAY_CONTROL_2 0x03
#define DISPLAY_OPTION 0x05
#define AUDIO_SYNC 0x06
#define BREATH_CONTROL_1 0x08
#define BREATH_CONTROL_2 0x09
#define SHUTDOWN 0x0A
#define AGC_CONTROL 0x0B
#define ADC_RATE 0x0C


#define INIT_PWM 0x20

#define BLINK_PERIOD 0x01
#define FADE_OUT_TIME 2
#define FADE_IN_TIME 2
#define EXTINGUISH_TIME 1

#define ISSI_TIMEOUT 100

#define DIGIT1_POSITION 6
#define DIGIT2_POSITION 7
#define DIGIT3_POSITION 8

bool IS31FL3731_write_reg(I2C_HandleTypeDef* handler, uint8_t reg, uint8_t data);
bool IS31FL3731_init(I2C_HandleTypeDef* handler);
bool IS31FL3731_send_buffer(I2C_HandleTypeDef* handler, uint8_t frameBuffer[11]);
bool IS31FL3731_send_buffer_PWM(I2C_HandleTypeDef* handler, uint8_t frameBuffer[11][8]);
bool IS31FL3731_send_buffer_BLINK(I2C_HandleTypeDef* handler, uint8_t blinkBuffer[11]);
bool IS31FL3731_write_PWM_reg(I2C_HandleTypeDef* handler, uint8_t start_reg, uint8_t data_buffer[8]);
bool IS31FL3731_clear_screen(I2C_HandleTypeDef* handler);
bool IS31FL3731_breath_control(I2C_HandleTypeDef* handler, uint8_t enable);
bool IS31FL3731_set_frame(I2C_HandleTypeDef* handler, uint8_t frame);
bool IS31FL3731_directLEDdrive(I2C_HandleTypeDef* handler, uint8_t pos, uint8_t aData);
char IS31FL3731_ascii_to_digit(char c);




#endif /* INC_INDICATION_IS31FL3731_H_ */
