/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

#include <avr/io.h>
#define F_CPU 16000000UL
#define true 1
#define  false 0
#define bool int

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

struct Clock
{
  int year ;//2100
  int month ; //12
  int date ;  // 31
  int day ;  //7
  int hour ; //24 hours
  int minutes ;  //60 minutes
  int seconds ;  //60 seconds
  int milli_sec ; //1000 millliseconds 
  int alarm1[3]; //HMS
  int alarm2[3]; //HMS
  bool alarm_on;

  
};
int* HHMM(struct Clock c)
{
    int x[4] = {c.hour / 10, c.hour % 10, c.minutes / 10, c.minutes % 10};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"
    return x;
#pragma clang diagnostic pop
};

int* SSmm(struct Clock c)
{
    int x[4] ={c.seconds/10,c.seconds%10},c.milli_sec/100,c.milli_sec/10-c.milli_sec%10;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"
    return  x;
#pragma clang diagnostic pop
}
void update_time(long int *count)
{
    ///////////////////////UPDATES TIMESTAMP////////////////////////////////
    if (TCNT1 > 62499) // 62499 --> 1 second(16MHz_prescalar_256)
    {
        *count += 1;
        TCNT1 = 0;
    }
    ////////////////////////////////////////////////////////////////////////
}

void beep(long int *count,int duration)// --> milliseconds
{
  PORTC |= (1<<PIN5);
  long int x = *count + duration;
  while(*count < (x))
  {
    update_time(count);
  }
  PORTC &= ~(1<<PIN5); 
}


void display(int *d)//   -->[H,H,M,M]
{
    PORTC |= (1 << PIN0)|(1 << PIN1)|(1 << PIN2)|(1 << PIN3); // SET ALL DIGITS HIGH ~(ACTIVE IS LOW)

    PORTC ^= (1 << PIN3) //     DIGIT LOW
    PORTB = digits[d[0]];   // DISPLAY digit set PortB(specific pins) HIGH
    PORTC ^= (1 << PIN3) //     DIGIT HIGH

    PORTC ^= (1 << PIN2) //     DIGIT LOW
    PORTB = digits[d[1]];   // DISPLAY digit set PortB(specific pins) HIGH
    PORTC ^= (1 << PIN2) //     DIGIT HIGH

    PORTC ^= (1 << PIN1) //     DIGIT LOW
    PORTB = digits[d[2]];   // DISPLAY digit set PortB(specific pins) HIGH
    PORTC ^= (1 << PIN1) //     DIGIT HIGH

    PORTC ^= (1 << PIN0) //     DIGIT LOW
    PORTB = digits[d[3]];   // DISPLAY digit set PortB(specific pins) HIGH
    PORTC ^= (1 << PIN0) //     DIGIT HIGH
}

void tick(struct Clock *t)
{
  if (TCNT1 > 62)//~1ms <62499 --> 1 second(16MHz_prescalar_256)> TODO make more accurate
    {
    t->milli_sec +=1; // increment time
    TCNT1 = 0; // reset timer
    }
  if(t->milli_sec >999)
  {
    t->seconds+=1;
    t->milli_sec = 0;
  }
  if(t->seconds >59)
  {
    t->minutes+=1;
    t->seconds = 0;
  }
  if (t->minutes >59)
  {
    t->hour +=1;
    t->minutes =0;
  }
  if (t->hour >23)
  {
    t->date +=1;
    t->hour =0;
  }
  if (t->date >31)
  {
    t->month+=1;
    t->date = 1;
  }
  if(t->month >12)
  {
    t->year +=1;
    t->month =1;
  }
  if(t->year > 2100)
  t->year = 0;
}

int main(void)
{
  TCCR1B |= (1 << CS12); //PRESCALER OF 256
  TCNT1 = 0;

  DDRB  |= (1 << PIN0) | (1 << PIN1) | (1 << PIN2) | (1 << PIN3) | (1 << PIN4) | (1 << PIN5);// OUTPUT
  DDRC  |= (1 << PIN5);// Buzzer
  DDRD  &= ~(1 << PIN0) | ~(1 << PIN1) | ~(1 << PIN2) | ~(1 << PIN3);// input BUTTONS
  DDRD  |= (1 << PIN6); //output 7SEG(g)

  PORTB &= ~(1 << PINB0) | ~(1 << PINB1) | ~(1 << PINB2) | ~(1 << PINB3) | ~(1 << PINB4) | ~(1 << PINB5) | ~(1 << PINB6);// 7SEG(a-f)

  long int count = 0;//  timestamp
  int alarm1[3]={8,33,00};
  int alarm2[3]={8,33,20};
  struct Clock c1 = {2018,8,18,1,8,32,40,123};
  
  
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
  do  //MAIN_LOOP
  {
    display(MMSS(c1));
    update_time(&count);
    
  }while(true);
#pragma clang diagnostic pop







































return 0;
}
