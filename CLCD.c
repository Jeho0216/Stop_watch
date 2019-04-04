/*
 * CLCD.c
 *
 * Created: 2019-04-01 오후 5:28:03
 *  Author: kccistc
 */
#include "CLCD.h"

#define PORT_DATA		PORTC
#define PORT_CONTROL	PORTB
#define DDR_DATA		DDRC
#define DDR_CONTROL		DDRB

#define RS_PIN		5
#define RW_PIN		6
#define E_PIN		7

#define COMMAND_CLEAR_DISPLAY	0X01
#define	COMMAND_8_BIT_MODE		0x38
#define COMMAND_4_BIT_MODE		0x28

#define COMMAND_DISPLAY_ON_OFF_BIT	2
#define COMMAND_CURSOR_ON_OFF_BIT	1
#define COMMAND_BLINK_ON_OFF_BIT	0

void LCD_pulse_enable(void){
	PORT_CONTROL |= (1 << E_PIN);
	_delay_ms(1);
	PORT_CONTROL &= ~(1 << E_PIN);
	_delay_ms(1);
}

void LCD_write_data(uint8_t data){
	PORT_CONTROL |= (1 << RS_PIN);
	PORT_DATA = data;
	LCD_pulse_enable();
	_delay_ms(2);
}

void LCD_write_command(uint8_t command){
	PORT_CONTROL &= ~(1 << RS_PIN);
	PORT_DATA = command;
	LCD_pulse_enable();
	_delay_ms(2);
}

void LCD_clear(void){
	LCD_write_command(COMMAND_CLEAR_DISPLAY);
	_delay_ms(2);
}

void LCD_init(void){
	_delay_ms(30);
	
	DDR_CONTROL = (1 << RS_PIN | 1 << RW_PIN | 1 << E_PIN);
	DDR_DATA = 0xFF;
	PORT_DATA = 0x00;
	
	_delay_ms(50);
	LCD_write_command(COMMAND_8_BIT_MODE);
	_delay_ms(5);
	LCD_write_command(COMMAND_8_BIT_MODE);
	_delay_ms(1);
	LCD_write_command(COMMAND_8_BIT_MODE);
	_delay_ms(1);
	LCD_write_command(0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT));
	_delay_ms(1);
	LCD_write_command(0x06);
	
	LCD_clear();
}

void LCD_write_string(char *string){
	uint8_t i;
	for(i = 0; string[i]; i++)
	LCD_write_data(string[i]);
}

void LCD_goto_XY(uint8_t row, uint8_t col){
	col %= 16;
	row %= 2;
	
	uint8_t address = (0x40 * row) + col;
	uint8_t command = 0x80 + address;
	
	LCD_write_command(command);
}