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

volatile int leading_edge = 0;
volatile int trailing_edge = 0;
volatile int period = 0;
volatile int overflow_num = 0;

volatile int tracker[5];
//tracker is an array which stores Morse code entries. 3 corresponds to dot, 4 corresponds to dash.

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
	TCCR1B &= ~(1<<ICES1);
	
	
	
	//Enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);
	
	//Clear interrupt flag
	TIFR1 |= (1<<ICF1);
	
	//Enable overflow interrupt
	TIMSK1 |= (1<<TOIE1);
	
	//Clear overflow flag
	TIFR1 |= (1<<TOV1);
	
	//Enable global interrupts
	sei();
}

ISR(TIMER1_OVF_vect)
{
	//Clear overflow flag
	//TIFR1 |= (1<<TOV1);
	overflow_num++;
	//sprintf(String,"The Overflow is %u\n", overflow_num);
	//UART_putstring(String);
}

ISR(TIMER1_CAPT_vect)
{

	if (!(TCCR1B & (1<<ICES1))) {
		//Obtain new leading edge time
		leading_edge = ICR1;
		}
	else {
		//Obtain new trailing edge time
		trailing_edge = ICR1;
		//Define period length
		period = trailing_edge + overflow_num*65535 - leading_edge;
		//Reset counter
		overflow_num = 0;
		}
		
		//Clear interrupt flag
		TIFR1 |= (1<<ICF1);
		
	//Toggle Edge Find
	TCCR1B ^= (1<<ICES1);

}

int main(void)
{
	Initialize();
	
	UART_init(BAUD_PRESCALER);
	
	DDRB |= (1<<DDB1); //Makes PB1 an output pin
	DDRB |= (1<<DDB2); //Makes PB2 an output pin
	
	while(1)
	{
		//dot (E)
		if(1875 < period && period < 12500 && tracker[0] == 0){                           
			tracker[0] = 3;
			period = 0;
	    }
		//dot dot (I)
		else if (1875 < period && period < 12500 && tracker[0] == 3 && tracker[1] == 0){          
			tracker[1] = 3;
			period = 0;
		}
		//dot dash (A)
		else if(12500 < period && period < 45000 && tracker[0] == 3 && tracker[1] == 0){         
			tracker[1] = 4;
			period = 0;
		}
		//dot dot dot (S)
		else if (1875 < period && period < 12500 && tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 0){         
			tracker[2] = 3;
			period = 0;
		}
		//dot dash dot (R)
		else if (1875 < period && period < 12500 && tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 0){         
			tracker[2] = 3;
			period = 0;
		}
		//dot dash dash (W)
		else if (12500 < period && period < 45000 && tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 0){
			tracker[2] = 4;
			period = 0;
		}
		//dot dot dash (U)
		else if (12500 < period && period < 45000 && tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 0){         
			tracker[2] = 4;
			period = 0;
		}
		//dot dot dot dot (H)
		else if (1875 < period && period < 12500 && tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 0){        
			tracker[3] = 3;
			period = 0;
		}
		//dot dot dot dash (V)
		else if (12500 < period && period < 45000 && tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 0){
			tracker[3] = 4;
			period = 0;
		}
		//dot dot dash dot (F)
		else if (1875 < period && period < 12500 && tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 0){
			tracker[3] = 3;
			period = 0;
		}
		//dot dash dot dot (L)
		else if (1875 < period && period < 12500 && tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 0){
			tracker[3] = 3;
			period = 0;
		}
		//dot dash dash dot (P)
		else if (1875 < period && period < 12500 && tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 0){
			tracker[3] = 3;
			period = 0;
		}
		//dot dash dash dash (J)
		else if (12500 < period && period < 45000 && tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 0){
			tracker[3] = 4;
			period = 0;
		}
		//dash (T)
		else if(12500 < period && period < 45000 && tracker[0] == 0){                           
			tracker[0] = 4;
			period = 0;
		}
		//dash dash (M)
		else if(12500 < period && period < 45000 && tracker[0] == 4 && tracker[1] == 0){
			tracker[1] = 4;
			period = 0;
		}
		//dash dot (N)
		else if(1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 0){
			tracker[1] = 3;
			period = 0;
		}
		//dash dot dash (K)
		else if (12500 < period && period < 45000 && tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 0){
			tracker[2] = 4;
			period = 0;
		}
		//dash dot dot (D)
		else if (1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 0){
			tracker[2] = 3;
			period = 0;
		}
		//dash dash dot (G)
		else if (1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 0){
			tracker[2] = 3;
			period = 0;
		}
		//dash dash dash (O)
		else if (12500 < period && period < 45000 && tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 0){
			tracker[2] = 4;
			period = 0;
		}	
		//dash dash dot dash (Q)
		else if (12500 < period && period < 45000 && tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 0){
			tracker[3] = 4;
			period = 0;
		}
		//dash dash dot dot (Z)
		else if (1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 0){
			tracker[3] = 3;
			period = 0;
		}
		//dash dot dash dash (Y)
		else if (12500 < period && period < 45000 && tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 0){
			tracker[3] = 4;
			period = 0;
		}
		//dash dot dot dash (X)
		else if (12500 < period && period < 45000 && tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 0){
			tracker[3] = 4;
			period = 0;
		}
		//dash dot dot dot (B)
		else if (1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 0){
			tracker[3] = 3;
			period = 0;
		}
		//dash dot dash dot (C)
		else if (1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 0){
			tracker[3] = 3;
			period = 0;
		}
		//dash dash dash dash dash (0)
		else if (12500 < period && period < 45000 && tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 0){
			tracker[4] = 4;
			period = 0;
		}
		//dot dash dash dash dash (1)
		else if (12500 < period && period < 45000 && tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 0){
			tracker[4] = 4;
			period = 0;
		}
		//dot dot dash dash dash (2)
		else if (12500 < period && period < 45000 && tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 0){
			tracker[4] = 4;
			period = 0;
		}
		//dot dot dot dash dash (3)
		else if (12500 < period && period < 45000 && tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 4 && tracker[4] == 0){
			tracker[4] = 4;
			period = 0;
		}
		//dot dot dot dot dash (4)
		else if (12500 < period && period < 45000 && tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 0){
			tracker[4] = 4;
			period = 0;
		}
		//dot dot dot dot dot (5)
		else if (1875 < period && period < 12500 && tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 0){
			tracker[4] = 3;
			period = 0;
		}
		//dash dot dot dot dot (6)
		else if (1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 0){
			tracker[4] = 3;
			period = 0;
		}
		//dash dash dot dot dot (7)
		else if (1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 0){
			tracker[4] = 3;
			period = 0;
		}
		//dash dash dash dot dot (8)
		else if (1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 3 && tracker[4] == 0){
			tracker[4] = 3;
			period = 0;
		}
		//dash dash dash dash dot (9)
		else if (1875 < period && period < 12500 && tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 0){
			tracker[4] = 3;
			period = 0;
		}
		
			
		if(overflow_num > 5){

			//Print A
 			if(tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 0 && tracker[3] == 0 && tracker[4] == 0){
				sprintf(String,"A \n");
				UART_putstring(String);
				tracker[0] = 0;
				tracker[1] = 0;
				tracker[2] = 0;
				tracker[3] = 0;
				tracker[4] = 0;
			 }
			 //Print B
			 if(tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 0){
				sprintf(String,"B \n");
				UART_putstring(String);
				tracker[0] = 0;
				tracker[1] = 0;
				tracker[2] = 0;
				tracker[3] = 0;
				tracker[4] = 0;
			 }
			 //Print C
			 if(tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 3 && tracker[4] == 0){
				 sprintf(String,"C \n");
				 UART_putstring(String);
				 tracker[0] = 0;
				 tracker[1] = 0;
				 tracker[2] = 0;
				 tracker[3] = 0;
				 tracker[4] = 0;
			 }
			 //Print D
			 if(tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 0 && tracker[4] == 0){
				 sprintf(String,"D \n");
				 UART_putstring(String);
				 tracker[0] = 0;
				 tracker[1] = 0;
				 tracker[2] = 0;
				 tracker[3] = 0;
				 tracker[4] = 0;
			 }
			 //Print E
			 if(tracker[0] == 3 && tracker[1] == 0 && tracker[2] == 0 && tracker[3] == 0 && tracker[4] == 0){
				 sprintf(String,"E \n");
				 UART_putstring(String);
				 tracker[0] = 0;
				 tracker[1] = 0;
				 tracker[2] = 0;
				 tracker[3] = 0;
				 tracker[4] = 0;
			 } 
			 //Print F
			 if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 3 && tracker[4] == 0){
				 sprintf(String,"F \n");
				 UART_putstring(String);
				 tracker[0] = 0;
				 tracker[1] = 0;
				 tracker[2] = 0;
				 tracker[3] = 0;
				 tracker[4] = 0;
			 }
			  //Print G
			  if(tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 0 && tracker[4] == 0){
				  sprintf(String,"G \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print H
			  if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 0){
				  sprintf(String,"H \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print I
			  if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 0 && tracker[3] == 0 && tracker[4] == 0){
				  sprintf(String,"I \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print J
			  if(tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 0){
				  sprintf(String,"J \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print K
			  if(tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 0 && tracker[4] == 0){
				  sprintf(String,"K \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print L
			  if(tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 0){
				  sprintf(String,"L \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print M
			  if(tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 0 && tracker[3] == 0 && tracker[4] == 0){
				  sprintf(String,"M \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print N
			  if(tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 0 && tracker[3] == 0 && tracker[4] == 0){
				  sprintf(String,"N \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print O
			  if(tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 0 && tracker[4] == 0){
				  sprintf(String,"O \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print P
			  if(tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 3 && tracker[4] == 0){
				  sprintf(String,"P \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print Q
			  if(tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 4 && tracker[4] == 0){
				  sprintf(String,"Q \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			  //Print R
			  if(tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 0 && tracker[4] == 0){
				  sprintf(String,"R \n");
				  UART_putstring(String);
				  tracker[0] = 0;
				  tracker[1] = 0;
				  tracker[2] = 0;
				  tracker[3] = 0;
				  tracker[4] = 0;
			  }
			   //Print S
			   if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 0 && tracker[4] == 0){
				   sprintf(String,"S \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print T
			   if(tracker[0] == 4 && tracker[1] == 0 && tracker[2] == 0 && tracker[3] == 0 && tracker[4] == 0){
				   sprintf(String,"T \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print U
			   if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 0 && tracker[4] == 0){
				   sprintf(String,"U \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print V
			   if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 4 && tracker[4] == 0){
				   sprintf(String,"V \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print W
			   if(tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 0 && tracker[4] == 0){
				   sprintf(String,"W \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print X
			   if(tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 4 && tracker[4] == 0){
				   sprintf(String,"X \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print Y
			   if(tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 0){
				   sprintf(String,"Y \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print Z
			   if(tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 0){
				   sprintf(String,"Z \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 1
			   if(tracker[0] == 3 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 4){
				   sprintf(String,"1 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 2
			   if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 4){
				   sprintf(String,"2 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 3
			   if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 4 && tracker[4] == 4){
				   sprintf(String,"3 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 4
			   if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 4){
				   sprintf(String,"4 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 5
			   if(tracker[0] == 3 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 3){
				   sprintf(String,"5 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 6
			   if(tracker[0] == 4 && tracker[1] == 3 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 3){
				   sprintf(String,"6 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 7
			   if(tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 3 && tracker[3] == 3 && tracker[4] == 3){
				   sprintf(String,"7 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 8
			   if(tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 3 && tracker[4] == 3){
				   sprintf(String,"8 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 9
			   if(tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 3){
				   sprintf(String,"9 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
			   //Print 0
			   if(tracker[0] == 4 && tracker[1] == 4 && tracker[2] == 4 && tracker[3] == 4 && tracker[4] == 4){
				   sprintf(String,"0 \n");
				   UART_putstring(String);
				   tracker[0] = 0;
				   tracker[1] = 0;
				   tracker[2] = 0;
				   tracker[3] = 0;
				   tracker[4] = 0;
			   }
		}		
				
 		if(1875 < period && period < 12500){
// 		sprintf(String,"DOT \n");
// 		UART_putstring(String);
 		PORTB |= (1 << PORTB1);
 		_delay_ms(50);
 		PORTB &= ~(1 << PORTB1);
 		//period = 0;
 		}
		else if (12500 < period && period < 45000){
// 		sprintf(String,"DASH \n");
// 		UART_putstring(String);
 		PORTB |= (1 << PORTB2);
 		_delay_ms(50);
 		PORTB &= ~(1 << PORTB2);
 		//period = 0;
 		}
		}
	}
