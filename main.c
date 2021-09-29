#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

#include "inc/uart.h"



char String[25];

volatile int edge = 0;
volatile int period = 0;
volatile int overflow_num = 0;

void Initialize()
{
	// Disable global interrupts
	cli();

	//Timer1 Setup
	//Set Timer 1 clock to be internal div by 256
	//62500Hz timer clock, 1 tick = (1/62500) second
	TCCR1B &= ~(1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS12);

	//Set Timer 1 to Normal
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);
	
	//set PB0 (ICP1 pin) to be input
	DDRB &= ~(1<<DDB0);
	//Makes PB5 an output pin
	DDRB |= (1<<DDB5);
	
	//Look for falling edge
	TCCR1B &= (1<<ICES1);
	//Clear interrupt flag
	TIFR1 |= (1<<ICF1);
	//Enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);
	
	//Enable overflow interrupt
	TIMSK1 |= (1<<TOIE1);
	//Clear overflow flag
	TIFR1 |= (1<<TOV1);
	
	//Enable global interrupts
	sei();
}

ISR(TIMER1_OVF_vect)
{
	overflow_num++;
}

ISR(TIMER1_CAPT_vect)
{
	//Clear interrupt flag
	TIFR1 |= (1<<ICF1);
	//Clear overflow flag
	TIFR1 |= (1<<TOV1);

	if (!(TCCR1B & (1<<ICES1))) {
		//Obtain new leading edge time
		edge = ICR1;
		}
	else {
		//Define period length
		period = ICR1 + overflow_num*0xFFFF - edge;
		//Reset counter
		overflow_num = 0;
	
	}
	sprintf(String,"The TCNT1 is %d\n", TCNT1);
	UART_putstring(String);
}
int main(void)
{
	Initialize();
	
	UART_init(BAUD_PRESCALER);
	
	while(1)
	{
		//sprintf(String,"The TCNT1 is %d\n", TCNT1);
		//UART_putstring(String);
		//_delay_ms(1000);
/*
		if(1875 < period && period < 12500){
		sprintf(String,"DOT \n");
		UART_putstring(String);
		}
		else if (12500 < period && period < 25000){
		sprintf(String,"DASH \n");
		UART_putstring(String);
		}
		else {
			while(!(PINB & (1<<PINB0)));
			_delay_ms(400);
			sprintf(String,"SPACE \n");
			UART_putstring(String);
		}
*/

/*
		sprintf(String,"Hello world! \n");
		UART_putstring(String);
		_delay_ms(1000);

		sprintf(String,"The year is %u\n", 2020);
		UART_putstring(String);
		_delay_ms(1000);
		
		sprintf(String, "ICR1 is %u\n", ICR1);
		UART_putstring(String);
		_delay_ms(1000);
*/
	}

}