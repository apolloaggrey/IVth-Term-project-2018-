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
#include <avr/interrupt.h>


#ifndef F_CPU          // if F_CPU was not defined in Project -> Properties
#define F_CPU 16000000UL     // define it now as 16 MHz unsigned long
#endif
#define true 1
#define  false 0
#define bool int
#define BIT_IS_SET(byte, bit) (byte & (1 << bit)) //logIcal 1 (ACTIVE LOW)
#define BIT_IS_CLEAR(byte, bit) (!(byte & (1 << bit))) //logical 0 (ACTIVE LOW)


volatile uint64_t count = 0;//  timestamp in millisecs
uint64_t last_press = 0;

bool set = false;
int mode = 0;
int mode2 = 0;


int year = 2019;//2100
int month = 2; //12
int date = 14;  // 31
int day = 2;  //7
int hour = 12; //24 hours
int minutes = 45;  //59 minutes
int seconds =  50; //59 seconds
int milliseconds = 0; //999 millliseconds
int alarm_hour = 12;
int alarm_minutes = 46;
bool alarm = false;
int hhmm[4];
int sscc[4];
int alarm_time[4];
int blank[4]={10,10,10,10};


void setup()
{
  //  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TCCR1B |= (1 << CS10); //NO PRESCALER
  TCCR1B |= (1 << WGM12); // MODE4 CTC
  OCR1A = 15999; // SET TOP (15999 --> 1 millisecond@(16MHz)> TODO make more accurate...)
  TIMSK1 |= (1 << OCIE1A); //Enable compare interrupt
  TCNT1 = 0;
  
  DDRB  &= ~(1 << PIN1) | ~(1 << PIN2) | ~(1 << PIN3) | ~(1 << PIN4); //clear DDRD bitS (1,2,3,4), sets PB[1:4](pins: 15,16,17,18) input BUTTONS)
  PORTB |= (1 << PB1)|(1 << PIN2)|(1 << PIN3)|(1 << PIN4);    // set PB[1:4]/PCINT[1:4] (pins: 15,16,17,18) internal pull-up resistor
  
  DDRC  |= (1 << PIN1) | (1 << PIN2) | (1 << PIN3) | (1 << PIN4) | (1 << PIN5); // Buzzer PC5 (pin 28) and 4digit PC[1:4](pin [24:27])select(ACTIVE LOW)
  PORTC |= (1 << PIN1) | (1 << PIN2) | (1 << PIN3) | (1 << PIN4) | (1 << PIN5); // ALL segs inactive
  PORTC &= ~(1 << PIN5);//alarm off
  
  DDRD  |= (1 << PIN0) | (1 << PIN1) | (1 << PIN2) | (1 << PIN3) | (1 << PIN4) | (1 << PIN5) | (1 << PIN6) | (1 << PIN7); //7SEG(a-f & .) OUTPUT
  PORTD &= ~(1 << PIN0) | ~(1 << PIN1) | ~(1 << PIN2) | ~(1 << PIN3) | ~(1 << PIN4) | ~(1 << PIN5) | ~(1 << PIN6)| ~(1 << PIN7);//

  /*
  PCICR - Pin Change Interrupt Control Register
  
  bit         7       6       5       4       3         2          1        0
  name        -       -       -       -       -       PCIE2      PCIE1    PCIE0
  set to      0       0       0       0       0         0          0        1
  
  bit 7 = 0
  bit 6 = 0
  bit 5 = 0
  bit 4 = 0
  bit 3 = 0
  PCIE2 = 0     don't enable pin change interrupt on pins PCINT23 through PCINT16
  PCIE1 = 0     don't enable pin change interrupt on pins PCINT14 through PCINT8
  PCIE0 = 1     enable pin change interrupt on pins PCINT0 through PCINT7
  */
  PCICR = 0b00000001;

  /*
  PCMSK0 - Pin Change Mask Register 0
  
  bit           7           6           5           4          3            2            1           0
  name        PCINT7     PCINT6       PCINT5     PCINT4      PCINT3      PCINT2         PCINT1     PCINT0
  set to        0           0           0           0           0           0            1           0
  
  PCINT7 = 0   don't enable PCINT7 pin change interrupt
  PCINT6 = 0   don't enable PCINT6 pin change interrupt
  PCINT5 = 0   don't enable PCINT5 pin change interrupt
  PCINT4 = 1   enable PCINT1 pin change interrupt
  PCINT3 = 1   enable PCINT3 pin change interrupt
  PCINT2 = 1   enable PCINT2 pin change interrupt
  PCINT1 = 1   enable PCINT1 pin change interrupt
  PCINT0 = 0   don't enable PCINT0 pin change interrupt
  */
  PCMSK0 = 0b00011110;
  
  sei();        // enable interrupts

  //  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


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


void HHMM()
{
  hhmm[0] = hour / 10;
  hhmm[1] = hour % 10;
  hhmm[2] = minutes / 10;
  hhmm[3] = minutes % 10;
  
};

void SSCC()
{
  sscc[0] =   seconds / 10;
  sscc[1] =   seconds % 10;
  sscc[2] =   milliseconds / 100;
  sscc[3] =   (milliseconds / 10) % 10;
}

void ALARM()
{
  alarm_time[0] = alarm_hour / 10;
  alarm_time[1] = alarm_hour % 10;
  alarm_time[2] = alarm_minutes / 10;
  alarm_time[3] = alarm_minutes % 10;
}


void beep(uint64_t ms) // -->TODO  milliseconds
{
  PORTC |= (1 << PIN5);
  uint64_t fin = count + ms;
  while (count < fin)
  {
    continue;
  }
  PORTC &= ~(1 << PIN5);
}


void display(int *ARRAY)//   -->[H,H,M,M]
{
  PORTC |= (1 << PIN1) | (1 << PIN2) | (1 << PIN3) | (1 << PIN4); // SET ALL DIGITS HIGH ~(ACTIVE IS LOW)
  for(int pos=1;pos<5;pos++)
  {
    PORTD = digits[ARRAY[4-pos]];   // DISPLAY digit set PortB(specific pins) HIGH
    PORTC &= ~(1 << pos ); //     DIGIT LOW
    delay(2);
    PORTC |= (1 << pos); //     DIGIT HIGH
  }

  if(!set)
  {
    if(milliseconds < 500)
    {
      PORTD = digits[20];   // DISPLAY digit set PortB(specific pins) HIGH
      PORTC &= ~(1 << 3 ); //     DIGIT LOW
      delay(1);
      PORTC |= (1 << 3); //     DIGIT HIGH
    }
  }
  else
  {
      PORTD = digits[20];   // DISPLAY digit set PortB(specific pins) HIGH
      PORTC &= ~(1 << 3 ); //     DIGIT LOW
      delay(1);
      PORTC |= (1 << 3); //     DIGIT HIGH
  }
  if(mode == 2)
  {
      PORTD = digits[20];   // DISPLAY digit set PortB(specific pins) HIGH
      PORTC &= ~(1 << 2 ); //     DIGIT LOW
      delay(1);
      PORTC |= (1 << 2); //     DIGIT HIGH
  }
}


ISR(PCINT0_vect) 
{
  
  if(BIT_IS_CLEAR(PINB, PB1)) //MODE if switch is pressed (logic low) 
  { 
    if(set == 0) 
    {
      mode ++;
    }
    else // set next item
    {
      mode2 ++;
      mode2 %=2; 
    }
//    PORTC |= (1 << PIN5);
  }
  
  else if(BIT_IS_CLEAR(PINB, PB2)) //SET if switch is pressed (logic low)
  {  
    set = !set;
//    PORTC ^= (1 << PIN5);
  }
  
  else if(BIT_IS_CLEAR(PINB, PB3)) //++ if switch is pressed (logic low)
  {  
    if(set)
    {
      switch(mode)
      {
        case 0: // hhmm
        {
          if(mode2==0)//hh
          {
            hour++;
          }
          if (mode2 ==1)//mm
          {
            minutes++;
          }
          break;
        }
        case 1:// sscc
        {
          if(mode2==0)//ss
          {
            seconds++;
          }
          if (mode2 ==1)//cc
          {
            milliseconds = 0;
          }
          break;
        }
        case 2: //ALARM hhmm
        {
          if(mode2==0)//hh
          {
            alarm_hour++;
          }
          if (mode2 ==1)//mm
          {
            alarm_minutes++;
          }
          break;
        }
      }
    }
//    PORTC &= ~(1 << PIN5);
  }
  
  else if(BIT_IS_CLEAR(PINB, PB4)) //-- if switch is pressed (logic low)
  {  
    alarm =false;
    if(set)
    {
      switch(mode)
      {
        case 0: // hhmm
        {
          if(mode2==0)//hh
          {
            hour--;
          }
          if (mode2 ==1)//mm
          {
            minutes--;
          }
          break;
        }
        case 1:// sscc
        {
          if(mode2==0)//ss
          {
            seconds--;
          }
          if (mode2 ==1)//cc
          {
            milliseconds = 0;
          }
          break;
        }
        case 2: // ALARM hhmm
        {
          if(mode2==0)//hh
          {
            alarm_hour--;
          }
          if (mode2 ==1)//mm
          {
            alarm_minutes--;
          }
          break;
        }
      }
    }
//    PORTC &= ~(1 << PIN5);
  }
}


ISR(TIMER1_COMPA_vect)// update seconds
{
  milliseconds ++; // increment time
  count ++;
  if (milliseconds > 999)
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
    day = (day % 7) + 1;
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
  if(alarm_hour > 23)alarm_hour = 0;
  if(alarm_minutes > 59)alarm_minutes = 0;

 ////////////////---ALARM_CHECK---/////////////////
  if ( seconds == 0)
  {
    alarm = true;
    for(int digit=0;digit<4;digit++)
    {
      if(hhmm[digit]!= alarm_time[digit])
      {
        alarm = false;
        break;
      }
    }  
  }
  ///////////////////////////////////////////////

  
  SSCC();
  HHMM();
  ALARM();
  
}


void delay(int ms)
{
  uint64_t fin = count + ms;
  while (count < fin)
  {
    continue;
  }
}


int main(void)
{
  setup();
     //MAIN_LOOP
  while(true)
  {
    /////////////__CHECKS__///////////////
    mode %= 3;// 5 modes

    if(alarm)
    {
      if(milliseconds<250)beep(1);
      else if(milliseconds >500)
      {
        if(milliseconds <750)beep(1);
      }
    }
    ///////////////////////////////////
    
    switch(mode)
    {
      case 0: // hours[24] - minutes
      {
        display(hhmm); 
        break;
      }
      case 1: // seconds - centiseconds
      {
        display(sscc); 
        break;
      }
      case 2: // ALARM (hours[24] - minutes)
      {
        display(alarm_time);
        break;
      }
      case 3: // timer
      {
        break;
      }
      case 4: // stopwatch
      {
        break;
      }
    }
  }


  return 0;
}
