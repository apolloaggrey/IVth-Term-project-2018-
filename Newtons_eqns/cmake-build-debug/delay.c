/*
 * push_button_led.c
 *
 * Created: 8/8/2018 2:58:54 PM
 *  Authors : Apollo Aggrey
 *        Madeline Ochieng
 *            Charles Mutua
 *Description: turn of LED while button is presses.
 */ 
#define F_CPU 16000000UL
#include <avr/io.h> 
#include <util/delay.h>

int main(void)
{
    DDRB  = 0b00000010;
    PORTB = 0b00000100;
    
    PORTB = 0b00000010;
    while (1) 
  {
    if (bit_is_clear(PORTB,1))//button is not pressed?
    {
//      PORTB &= ~(1<<PINB2); //switch led 0ff
       PORTB = 0b00000000;
      _delay_ms(170); 
    }
    else
    {
      PORTB &= ~(1<<PINB2); //switch led 0ff
       PORTB = 0b0000100;
      _delay_ms(170);
     }
  }
}

