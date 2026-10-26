/*
 * project.c
 *
 *  Created on: Sep 1, 2026
 *      Author: Mohammed Yasser
 */
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>


//Global Variables
unsigned char sec = 0;
unsigned char min = 0;
unsigned char hour = 0;
unsigned char mode = 0; // Count-up = 0 , Count-down = 1;
unsigned char target_hour = 0;
unsigned char target_min = 0;
unsigned char target_sec = 0;
unsigned char en_change = 0;
unsigned char ticks = 0;
unsigned char display_digits[6];
unsigned char digit = 0;
unsigned char cd_hour = 0;
unsigned char cd_min = 0;
unsigned char cd_sec = 0;
unsigned char setting = 0;


void TIMER1_init(){
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) |(1 << CS12) | (1 << CS10);
	OCR1A = 15624; //Every 1 sec get interrupt.
	TIMSK |= (1 << OCIE1A);

}

ISR(TIMER1_COMPA_vect){
	if(mode == 0){
		sec++;
		if(sec == 60){
			min++;
			sec = 0;
		}
		if(min == 60){
			hour++;
			min = 0;
		}
	} else      // Count-down
	{
		if (cd_hour == 0 &&
				cd_min == 0 &&
				cd_sec == 0)
		{
			// Reached 00:00:00
			PORTD |= (1 << PD0);   // Buzzer ON

			// Stop Timer
			TCCR1B &= ~((1 << CS12) |
					(1 << CS11) |
					(1 << CS10));
		}
		else if (cd_sec > 0)
		{
			cd_sec--;
		}
		else
		{
			cd_sec = 59;
			if (cd_min > 0)
			{
				cd_min--;
			}
			else
			{
				cd_min = 59;
				cd_hour--;
			}
		}
	}
}




void INT0_init(){
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);
	MCUCR |= (1 << ISC01);
	GICR |= (1 << INT0);
}
void INT1_init(){
	DDRD &= ~(1 << PD3);
	MCUCR |= (1 << ISC11) | (1 << ISC10);
	GICR |= (1 << INT1);
}
void INT2_init(){
	DDRB &= ~(1 << PB2);
	PORTB |= (1 << PB2);
	MCUCSR &= ~(1 << ISC2);
	GICR |= (1 << INT2);
}

ISR(INT0_vect){ // Reset
	TCNT1 = 0;
	if(mode == 0){
		sec = 0;
		min = 0;
		hour = 0;
	} else {
		cd_hour = target_hour;
		cd_min = target_min;
		cd_sec = target_sec;
	}
	PORTD &= ~(1 << PD0);
}

ISR(INT1_vect){ //Pause
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
	en_change = 1;
	PORTD &= ~(1 << PD0);
}

ISR(INT2_vect){ //Resume
	TCCR1B |= (1 << CS12) | (1 << CS10);
	en_change = 0;
	if(setting == 1){
		TCNT1 = 0;
		cd_hour = target_hour;
		cd_min = target_min;
		cd_sec = target_sec;
		setting = 0;
	}
}


void display(unsigned char h, unsigned char m, unsigned char s)
{
	display_digits[0] = h / 10;
	display_digits[1] = h % 10;

	display_digits[2] = m / 10;
	display_digits[3] = m % 10;

	display_digits[4] = s / 10;
	display_digits[5] = s % 10;

	PORTA &= ~0x3F;
	PORTA |= (1 << digit);

	PORTC &= ~0x0F;
	PORTC |= display_digits[digit];

	digit++;
	_delay_ms(2);
	if (digit == 6)
	{
		digit = 0;
	}
}



int main(void)
{
	TIMER1_init();
	INT0_init();
	INT1_init();
	INT2_init();

	sei();
	DDRB &= ~((1 << PB0) | // dec hours
			(1 << PB1) | // inc hours
			(1 << PB3) | // dec min
			(1 << PB4) | // inc min
			(1 << PB5) | // dec sec
			(1 << PB6) | // inc sec
			(1 << PB7)); // mode
	PORTB |= (1 << PB0) |
			(1 << PB1) |
			(1 << PB3) |
			(1 << PB4) |
			(1 << PB5) |
			(1 << PB6) |
			(1 << PB7);

	DDRD |= (1 << PD0) //Buzzer
   																			| (1 << PD4) //Count-up led
																			| (1 << PD5); // Count-down led
	PORTD &= ~((1 << PD0) |
			(1 << PD4) |
			(1 << PD5));

	DDRA |= (1 << PA0) |
			(1 << PA1) |
			(1 << PA2) |
			(1 << PA3) |
			(1 << PA4) |
			(1 << PA5);
	PORTA &= ~((1 << PA0) |
			(1 << PA1) |
			(1 << PA2) |
			(1 << PA3) |
			(1 << PA4) |
			(1 << PA5));

	DDRC |= 0x0f;
	PORTC &= ~0x0f;


	while (1)
	{


		//display
		if(mode == 0){
			setting = 0;
			display(hour, min, sec);
			PORTD |= (1<<PD4);
			PORTD &=~(1<<PD5);
			PORTD &= ~(1 << PD0);
		}else {
			if (setting == 1){
				display(target_hour, target_min, target_sec);
			}
			else{
				display(cd_hour, cd_min, cd_sec);
			}
			PORTD |= (1 << PD5);
			PORTD &= ~(1 << PD4);
		}
		if(en_change == 1){
			if(!(PINB & (1<<PB7))){
				mode ^= 1;
				if(mode == 0){
					sec = 0;
					min = 0;
					hour = 0;
				} else {
					cd_hour = target_hour;
					cd_min = target_min;
					cd_sec = target_sec;
					target_hour = 0;
					target_min = 0;
					target_sec = 0;
				}
				while(!(PINB & (1<<PB7)));
			}
			//change area
			if(mode == 1){
				if(!(PINB & (1 << PB0))){
					if(target_hour > 0)
						target_hour--;
					setting = 1;
					while(!(PINB & (1 << PB0)));
				}
				if(!(PINB & (1 << PB1))){
					if(target_hour < 99)
						target_hour++;
					setting = 1;
					while(!(PINB & (1 << PB1)));
				}
				if(!(PINB & (1 << PB3))){
					if(target_min > 0)
						target_min--;
					setting = 1;
					while(!(PINB & (1 << PB3)));
				}
				if(!(PINB & (1 << PB4))){
					if(target_min < 59)
						target_min++;
					setting = 1;
					while(!(PINB & (1 << PB4)));
				}
				if(!(PINB & (1 << PB5))){
					if(target_sec > 0)
						target_sec--;
					setting = 1;
					while(!(PINB & (1 << PB5)));
				}
				if(!(PINB & (1 << PB6))){
					if(target_sec < 59)
						target_sec++;
					setting = 1;
					while(!(PINB & (1 << PB6)));
				}
			}
		}
	}
}



