/*
 * GccApplication1.c
 *
 * Created: 2014-11-26 17:21:05
 *  Author: Adriana Borowa, Micha³ Kowalik
 */ 

#define F_CPU 16000000L
#define DISPLAY_PORT PORTA
#define DISPLAY_DDR DDRA
#define DISPLAY_GRD_PORT PORTB
#define DISPLAY_GRD_DDR DDRB

#include <avr/io.h>
#include <util/delay.h>
#include "task_manager.h"

uint8_t CYFRA[10] = {0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000};


ISR(TIMER0_COMP_vect){
	schedule();
}

volatile uint32_t actual_time = 0;
volatile uint8_t actual_display = 0;

/**
* Dummy task functions #1
*/
void increment_value(void* args){
	actual_time++;
}

/**
* Dummy task functions #2
*/
void change_display(void* args){
	DISPLAY_GRD_PORT = 0xff;
	actual_display = (actual_display + 1) % 4;
	if (actual_display==0) DISPLAY_PORT = CYFRA[actual_time%10];
	else if (actual_display==1) DISPLAY_PORT = CYFRA[(actual_time/10)%10];
	else if (actual_display==2) DISPLAY_PORT = CYFRA[(actual_time/100)%10];
	else if (actual_display==3) DISPLAY_PORT = CYFRA[(actual_time/1000)%10];
	DISPLAY_GRD_PORT = (~(1<<actual_display));
	
}

/**
* Setup atmega32's clock for interrupts every 1ms
*/
void setupTimer(){
	
	TCCR0 |= (1<<WGM01) | (0<<WGM00);	// set clock type as CTC
	OCR0 = 250;							// set Output Compare Register - together with prescaler this will give us
	// interrupt every 1ms
	sei();								// turn interrupts on
	TIMSK |= (1<<OCIE0);				// set interrupts co compare
	TCCR0 |= (1<<CS00) | (1<<CS01 );	// set clock prescaler at 64*250 = 16,000; 16MHz * 16,000 = 1KHZ;
	
}





int main(void)
{
	DISPLAY_DDR = 0xFF;
	DISPLAY_PORT = 0x0;
	DISPLAY_GRD_DDR = DISPLAY_GRD_DDR | 0x0F;
	DISPLAY_GRD_PORT = DISPLAY_GRD_PORT & 0xFF;
	setupTimer();
	addTask(0, 100, increment_value, (void*)0);
	addTask(1, 4, change_display, (void*)0);
	execute();
	
	for(int i=0;i<10;i++){
		DISPLAY_PORT = CYFRA[i];
		_delay_ms(1000);
	}
}