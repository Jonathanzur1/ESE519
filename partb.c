#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

int edge1 = 0;
int edge2 = 0;

void Initialize()
{
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

}

int main(void)
{
	//set PB0 (ICP1 pin) to be input
	DDRB &= ~(1<<DDB0);
	//Makes PB5 an output pin
	DDRB |= (1<<DDB5);
	Initialize();
	
	while(1)
	{
		if (PINB & (1<<PINB0)) {
		PORTB &= ~(1<<PORTB5);
		}
		else {
		PORTB |= (1<<PORTB5);	
	}
	}
	
}