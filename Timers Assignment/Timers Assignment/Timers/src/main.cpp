#include <Arduino.h>

bool isON = false;
int prescaler = 1024;
bool buttonRelease = false;

ISR(PCINT2_vect)
{
  if (!(PIND & (1 << PIND7)))
  {
    isON = !isON;
  }
  PORTB &= ~(1 << PB0);
}

ISR(PCINT0_vect)
{
  buttonRelease = !(PINB & (1 << PINB1));
}

ISR(TIMER1_COMPA_vect)
{
  if(isON)
  {
    PORTB ^= (1 << PB0);
  }
}

void setup()
{
  // put your setup code here, to run once:

  //Initiate timer1

  // disable all interrupts
  noInterrupts();

  TCCR1A = 0;
  TCCR1B = 0;

  //Pin mode
  DDRD |= (1 << DDD7);
  DDRB &= ~(1 << DDB0);
  DDRB |= (1 << DDB1);
  
  //Set pin 8 to off
  PORTB &= ~(1 << PB0);

  //Enable the interrupts for port D
  PCICR |= (1 << PCIE2);
  //Enable the interrupts for port B
  PCICR |= (1 << PCIE0);

  //Enable interreupts for pin 7
  PCMSK2 |= (1 << PCINT23);
  //Enable interrupts for pin 9
  PCMSK0 |= (1 << PCINT1);

  //Set timer frequency
  OCR1A = 62500;

  //Set timer mode
  TCCR1B &= ~(1 << WGM13) & ~(1 << WGM12) & ~(1 << WGM11) & ~(1 << WGM10);

  //Set prescaler to 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TCCR1B &= ~(1 << CS11);

  //Enable timer1 compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  //Enable all interrupts
  interrupts();
}

void loop()
{
  // put your main code here, to run repeatedly:

    //Set prescaler to 256 from 1024
  if(buttonRelease && prescaler == 1024)
  {
      TCCR1B &= ~(1 << WGM13) & ~(1 << WGM12) & ~(1 << WGM11) & ~(1 << WGM10);

      TCCR1B |= (1 << CS12);
      TCCR1B &= ~(1 << CS11) & ~(1 << CS10);
      
      prescaler = 256;
      buttonRelease = false;
  }
  //Set prescaler to 64 from 256
  else if(buttonRelease && prescaler == 256)
  {
      TCCR1B &= ~(1 << WGM13) & ~(1 << WGM12) & ~(1 << WGM11) & ~(1 << WGM10);

      TCCR1B |= (1 << CS11) | (1 << CS10);
      TCCR1B &= ~(1 << CS12);

      prescaler = 64;
      buttonRelease = false;
  }
  //Set prescaler to 8 from 64
  else if(buttonRelease && prescaler == 64)
  {
      TCCR1B &= ~(1 << WGM13) & ~(1 << WGM12) & ~(1 << WGM11) & ~(1 << WGM10);

      TCCR1B |= (1 << CS11);
      TCCR1B &= ~(1 << CS12) & ~(1 << CS10);

      prescaler = 8;
      buttonRelease = false;
  }
  else if(buttonRelease && prescaler == 8)
  {
      TCCR1B &= ~(1 << WGM13) & ~(1 << WGM12) & ~(1 << WGM11) & ~(1 << WGM10);

      TCCR1B |= (1 << CS12) | (1 << CS10);
      TCCR1B &= ~(1 << CS11);

      prescaler = 1024;
      buttonRelease = false;
  }
}
