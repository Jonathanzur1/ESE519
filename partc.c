#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

int edge1 = 0;
int edge2 = 0;

void Initialize()
{
	// Disable global interrupts
	cli();

	//Timer1 Setup
	//Set Timer 1 clock to be internal div by 8
	//2MHz timer clock, 1 tick = (1/2M) second
	TCCR1B &= ~(1<<CS10);
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);

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
	
	//Enable global interrupts
	sei();
}


ISR(TIMER1_CAPT_vect)
{
	//Clear interrupt flag
	TIFR1 |= (1<<ICF1);
	//Toggle which edge you are looking for
	TCCR1B ^= (1<<ICES1);
	//Toggle the LED based on which edge is found!
	PORTB ^= (1<<PORTB5);
	
	//Delete the first two lines of code in this function if you would like the button to toggle on press.
}
int main(void)
{
	Initialize();	
	while(1)
	{
	}
}