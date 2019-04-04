/*
 * CLCD.h
 *
 * Created: 2019-04-01 오후 5:28:12
 *  Author: kccistc
 */ 


#ifndef CLCD_H_
#define CLCD_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void LCD_pulse_enable();
void LCD_write_data(uint8_t data);
void LCD_write_command(uint8_t command);
void LCD_clear();
void LCD_init();
void LCD_write_string(char *string);
void LCD_goto_XY(uint8_t row, uint8_t col);

#endif /* CLCD_H_ */