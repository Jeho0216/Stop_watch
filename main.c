/*
 * stop_watch.c
 *
 * Created: 2019-04-04 오후 2:14:58
 * Author : kccistc
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "CLCD.h"
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

volatile int count_0 = 0;		//타이머/카운터0에서 사용하는 count
volatile int count_2 = 0;		//타이머/카운터2에서 사용하는 count
volatile int stop_watch_val = 0;
volatile uint8_t lap_count = 0;
volatile int lap_time[4] = { 0, 0, 0, 0 };	

void INT_init(){
	EIMSK = (1 << INT0 | 1 << INT1 | 1 << INT2);
	EICRA = (1 << ISC01 | 1 << ISC11 | 1 << ISC21);	//외부인터럽트0 사용 및 하강엣지에서 인터럽트 발생.
	
	TCCR0 = (1 << CS02);		//타이머/카운터0 의 분주비는 64
	TCCR2 = (1 << CS21);		//타이머/카운터2의 분주비는 64
	OCR0 = 250;
	OCR2 = 250;				//비교일치 값
	TIMSK = (1 << OCIE2);		//비교일치 인터럽트 0, 2 허용O
	sei();		//전역적 인터럽트 허용.
}

void print_lcd_init(){
	LCD_goto_XY(0, 0);
	LCD_write_string("1.");
	LCD_goto_XY(0, 7);
	LCD_write_string("2.");
	LCD_goto_XY(1, 0);
	LCD_write_string("3.");
	LCD_goto_XY(1, 7);
	LCD_write_string("4.");
	LCD_goto_XY(0, 2);
}

void display_digit(int position, int number){
	uint8_t numbers[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x67};

	PORTE |= 0xF0;
	PORTE &= ~(1 << position);
	PORTF = numbers[number];
}

void save_lap_time(){
	lap_time[lap_count - 1] = stop_watch_val;
}

void print_lap_time(){
	int i;
	char buff[5] = { 0, 0, 0, 0, 0 };
	for(i = 0; i < 4; i++){
		sprintf(buff, "%4d", lap_time[i]);
		
		switch(i){
			case 0:
				LCD_goto_XY(0, 2);
				printf("%s\t", buff);
				LCD_write_string(buff);
				break;
			case 1:
				LCD_goto_XY(0, 9);
				printf("%s\t", buff);
				LCD_write_string(buff);
				break;
			case 2:
				LCD_goto_XY(1, 2);
				printf("%s\t", buff);
				LCD_write_string(buff);
				break;
			case 3:
				LCD_goto_XY(1, 9);
				printf("%s\t", buff);
				LCD_write_string(buff);
				break;
		}
	}
	printf("\n");
}

ISR(TIMER0_COMP_vect){		//0.001초마 인터럽트 발생(OCR0 = 250)
	count_0++;
	TCNT0 = 0x00;
	if(count_0 >= 100){		//0.001초가 100번 발생 -> 0.1초마다 실행
		stop_watch_val++;
		if(stop_watch_val > 9999)
		stop_watch_val = 0;
		count_0 = 0;
	}
}

ISR(TIMER2_COMP_vect){		//0.001초마다 인터럽트 발생(OCR2 = 250)
	static uint8_t position = 0;
	TCNT2 = 0x00;
	count_2++;
	if(count_2 >= 10){
		switch(position){
			case 0:
				display_digit(4, stop_watch_val / 1000);
				break;
			case 1:
				display_digit(5, stop_watch_val % 1000 / 100);
				break;
			case 2:
				display_digit(6, stop_watch_val % 100 / 10);
				break;
			case 3:
				display_digit(7, stop_watch_val % 10);
				break;
		}
		position++;
		if(position >= 4)
			position = 0;
		count_2 = 0;
	}
}

ISR(INT0_vect){
	TIMSK ^= (1 << OCIE0);
}

ISR(INT1_vect){		//리셋버튼
	if((TIMSK & (0x01 << OCIE0)) == 0){		//일시정지 상황
		stop_watch_val = 0;
		for(int i = 0; i < 4; i++)		//랩타임 초기화
			lap_time[i] = 0;
		lap_count = 0;
		print_lcd_init();
		print_lap_time();
	}
}

ISR(INT2_vect){
	if((TIMSK & (0x01 << OCIE0)) != 0){
		lap_count++;
		
		if(lap_count > 4)
			lap_count = 1;
		save_lap_time();
		print_lap_time();
	}
}

int main(void){
	DDRE = 0xFF;
	DDRF = 0xFF;
	PORTF = 0x00;
	
	LCD_init();
	LCD_clear();
	UART0_init();
	INT_init();		//인터럽트 초기화
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	print_lcd_init();
	print_lap_time();
	while (1){
		
	}
}

