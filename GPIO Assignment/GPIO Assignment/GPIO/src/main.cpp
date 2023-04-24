#include <Arduino.h>
bool button_1_down = false;
bool button_2_down = false;
unsigned long button_1_down_start = 0;
unsigned long button_2_down_start = 0;
bool button_1_pressed = false;
bool button_2_pressed = false;
unsigned long button_1_pressed_millis = 0;
unsigned long button_2_pressed_millis = 0;
ISR(PCINT0_vect)
{
  // Remove interrupt flag
  PCIFR |= (1 << PCIF0);
  button_1_down = (PINB & (1 << 0)) == 0;
  button_2_down = (PINB & (1 << 1)) == 0;
  if (button_1_down)
  {
    button_1_down_start = millis();
    button_1_pressed = false;
    button_1_pressed_millis = 1;
  }
  else if (button_1_pressed_millis == 1)
  {
    button_1_pressed = true;
    button_1_pressed_millis = millis() - button_1_down_start;
  }
  if (button_2_down)
  {
    button_2_down_start = millis();
    button_2_pressed = false;
    button_2_pressed_millis = 1;
  }
  else if (button_2_pressed_millis == 1)
  {
    button_2_pressed = true;
    button_2_pressed_millis = millis() - button_2_down_start;
  }
}
unsigned long millis_target_led_1 = 0;
unsigned long millis_target_led_2 = 0;
void setup()
{
  // Put your setup code here, to run once:
  Serial.begin(9600);
  // Apply pin modes
  DDRD |= (1 << 6);
  DDRD |= (1 << 7);
  PORTD &= ~(1 << 6);
  PORTD &= ~(1 << 7);
  DDRB &= ~(1 << 0);
  DDRB &= ~(1 << 1);
  PORTB &= ~(1 << 0);
  PORTB &= ~(1 << 1);
  // Enable pin change interrupts for the required port
  PCICR |= (1 << PCIE0);
  // Add button pin to pin change mask register
  PCMSK0 |= (1 << PCINT0);
  PCMSK0 |= (1 << PCINT1);
}
void loop()
{
  // Put your main code here, to run repeatedly:
  if (millis() >= millis_target_led_1)
  {
    if (button_1_pressed)
    {
      if (button_1_pressed_millis > 500)
      {
        millis_target_led_1 = millis() + 1000;
      }
      else if (button_1_pressed_millis > 20)
      {
        millis_target_led_1 = millis() + 200;
      }
      if (button_1_pressed_millis > 0)
      {
        PORTD ^= (1 << 6);
      }
    }
  }
  if (millis() >= millis_target_led_2)
  {
    if (button_2_pressed)
    {
      if (button_2_pressed_millis > 500)
      {
        millis_target_led_2 = millis() + 1000;
      }
      else if (button_2_pressed_millis > 20)
      {
        millis_target_led_2 = millis() + 200;
      }
      if (button_2_pressed_millis > 0)
      {
        PORTD ^= (1 << 7);
      }
    }
  }
}