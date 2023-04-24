//Sources:
//https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
//https://www.instructables.com/Introduction-to-ADC-in-AVR-Microcontroller-for-Beg/

#include <Arduino.h>

uint16_t analog = 0u;

ISR(ADC_vect)
{
  analog = 0u;
  analog |= ADCH;
}

void setup() 
{
  // put your setup code here, to run once:

  //Set pin 6 to output
  DDRD |= (1 << DDD6);

  //Set Analog 2 to input
  DDRC &= ~(1 << PC2);
    
  //Set ADC to Freerunning mode
  ADCSRB &= ~(1 << ADTS2);
  ADCSRB &= ~(1 << ADTS1);
  ADCSRB &= ~(1 << ADTS0);

  //Set ADC prescalar to 128
  ADCSRA |= (1 << ADPS2);
  ADCSRA |= (1 << ADPS1);
  ADCSRA |= (1 << ADPS0);

  //Set voltage refference to AVcc
  ADMUX &= ~(1 << REFS1);
  ADMUX |= (1 << REFS0);

  //Left adjust the result
  ADMUX |= (1 << ADLAR);

  //Set analog channel to ADC2
  ADMUX &= ~(1 << MUX3);
  ADMUX &= ~(1 << MUX2);
  ADMUX |= (1 << MUX1);
  ADMUX &= ~(1 << MUX0);

  //Enable ADC Auto Trigger
  ADCSRA |= (1 << ADATE);

  //Enable ADC Interrupt
  ADCSRA |= (1 << ADIE);

  //Enable ADC
  ADCSRA |= (1 << ADEN);

  //Start ADC
  ADCSRA |= (1 << ADSC);

  //Set pin 6 ON
  PORTD |= (1 << PORTD6);

  //Set COM1A to compare mode
  TCCR0A |= (1 << COM0A1);

  //Set timer mode to fast PWM
  TCCR0A |= (1 << WGM00);
  TCCR0A |= (1 << WGM01);
}

void loop() 
{
  // put your main code here, to run repeatedly:

  //Change the LED brightness
  OCR0A = analog;
}