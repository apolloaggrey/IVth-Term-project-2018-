/*
VERSION 1









*/

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define true 1
#define  false 0
#define bool int
 int count = 0;//  timestamp in millisecs
 int last_press=0;


int year = 2019;//2100
int month = 2; //12
int date = 14;  // 31
int day = 2;  //7
int hour = 0; //24 hours
int minutes = 0;  //60 minutes
int seconds = 0;  //60 seconds
int milliseconds = 0; //10 millliseconds
int alarm1[3]; //HMS
int alarm2[3]; //HMS
bool alarm_on = false;
int x[4];

int  digits[21] =
{
  0b00111111, // DISPLAY ->00
  0b00000110, // DISPLAY ->01
  0b01011011, // DISPLAY ->02
  0b01001111, // DISPLAY ->03
  0b01100110, // DISPLAY ->04
  0b01101101, // DISPLAY ->05
  0b01111101, // DISPLAY ->06
  0b00000111, // DISPLAY -> 7
  0b01111111, // DISPLAY -> 8
  0b01101111, // DISPLAY -> 9
  0b00000000, // DISPLAY _ 10
  0b01110110, // DISPLAY H 11
  0b01111001, // DISPLAY E 12
  0b00111000, // DISPLAY L 13
  0b01011100, // DISPLAY O 14
  0b01110111, // DISPLAY A 15
  0b01010100, // DISPLAY n 16
  0b01011110, // DISPLAY d 17
  0b01010000, // DISPLAY r 18
  0b01101110, // DISPLAY y 19
  0b10000000, // DISPLAY . 20
};



 int* HHMM()
{
  x[0] = hour / 10;
  x[1] = hour % 10;
  x[2] = minutes / 10;
  x[3] = minutes % 10;
  return x ;
};


 int* MMSS()
{
  x[0] = minutes / 10;
  x[1] = minutes % 10;
  x[2] = seconds / 10;
  x[3] = seconds % 10;
  
  return x ;
};


 int* SSCC()
{
  x[0] =   seconds / 10;
  x[1] =   seconds % 10;
  x[2] =   milliseconds / 10;
  x[3] =   milliseconds %10;
  return  x;
}


void beep(int duration) // -->TODO  milliseconds
{
  PORTC |= (1 << PIN5);
   int y = count + duration;
  while (count < y)
  {
    tick();
    continue;
  }
  PORTC &= ~(1 << PIN5);
}


void display( int *ARRAY)//   -->[H,H,M,M]
{
PORTC |= (1 << PIN4) | (1 << PIN1) | (1 << PIN2) | (1 << PIN3); // SET ALL DIGITS HIGH ~(ACTIVE IS LOW)
  
PORTD = digits[ARRAY[3]];   // DISPLAY digit set PortB(specific pins) HIGH
  PORTC &= ~(1 << 1 ); //     DIGIT LOW
  PORTC |= (1 << 1); //     DIGIT HIGH
  
PORTD = digits[ARRAY[2]];   // DISPLAY digit set PortB(specific pins) HIGH
  PORTC &= ~(1 << 2 ); //     DIGIT LOW
  PORTC |= (1 << 2); //     DIGIT HIGH
  
PORTD = digits[ARRAY[1]];   // DISPLAY digit set PortB(specific pins) HIGH
  PORTC &= ~(1 << 3 ); //     DIGIT LOW
  PORTC |= (1 << 3); //     DIGIT HIGH
  
PORTD = digits[ARRAY[0]];   // DISPLAY digit set PortB(specific pins) HIGH
  PORTC &= ~(1 << 4 ); //     DIGIT LOW
  PORTC |= (1 << 4); //     DIGIT HIGH
}


bool is_set(int port,int pin)
{
  last_press = count;
  if(count-last_press < 1)
    return false;
  else
  {
    if(port & (1<<pin)) // use mask to check reg val...
    return true;
    return false; 
  }
}


void tick()
{
  if (TCNT1 > 625)//~10ms <62499 --> 1 second(16MHz_prescalar_256)> TODO make more accurate
  {
    milliseconds += 1; // increment time
    TCNT1 = 0; // reset timer
    MMSS();
  }
  if (milliseconds > 100)
  {
    seconds += 1;
     milliseconds = 0;
  }
  if ( seconds > 59)
  {
     minutes += 1;
     seconds = 0;
  }
  if ( minutes > 59)
  {
     hour += 1;
     minutes = 0;
  }
  if ( hour > 23)
  {
     date += 1;
     hour = 0;
  }
  if ( date > 31)
  {
     month += 1;
     date = 1;
  }
  if ( month > 12)
  {
     year += 1;
     month = 1;
  }
}


int main(void)
{
//  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TCCR1B |= (1 << CS12); //PRESCALER OF 256
  TCNT1 = 0;

  DDRD  |= (1 << PIN0) | (1 << PIN1) | (1 << PIN2) | (1 << PIN3) | (1 << PIN4) | (1 << PIN5) | (1 << PIN6) | (1 << PIN7); // OUTPUT
  DDRC  |= (1 << PIN1) | (1 << PIN2) | (1 << PIN3) | (1 << PIN4) | (1 << PIN5); // Buzzer and 4digit select(ACTIVE LOW)
  DDRB  &= ~(1 << PIN1) | ~(1 << PIN2) | ~(1 << PIN3) | ~(1 << PIN4); // input BUTTONS)

  PORTD &= ~(1 << PIN0) | ~(1 << PIN1) | ~(1 << PIN2) | ~(1 << PIN3) | ~(1 << PIN4) | ~(1 << PIN5) | ~(1 << PIN6);// 7SEG(a-f)
  PORTC |= (1 << PIN1) | (1 << PIN2) | (1 << PIN3) | (1 << PIN4) | (1 << PIN5); // ALL segs inactive
  PORTC &= ~(1 << PIN5);//alarm off
  
//  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  do  //MAIN_LOOP
  {
    tick();
    display(x);
  } 
  while (true);








































  return 0;
}

