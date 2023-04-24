#include <Arduino.h>
#include <String.h>

#define BAUD 9600
#define MYUBRR ((F_CPU / 16L / BAUD) - 1)

void setup() {
  // put your setup code here, to run once:

  // Set baud rate to 9600
  UBRR0H = (unsigned char)(MYUBRR >> 8);
  UBRR0L = (unsigned char)MYUBRR;
  
  // Enable transmitter and receiver
  UCSR0B |= (1 << RXEN0);
  UCSR0B |= (1 << TXEN0);

  // Set asynchronous USART
  UCSR0C &= ~(1 << UMSEL01);
  UCSR0C &= ~(1 << UMSEL00);

  // Set frame format to 8data
  UCSR0B &= ~(1 << UCSZ02);
  UCSR0C |= (1 << UCSZ01);
  UCSR0C |= (1 << UCSZ00);

  // Set frame format to 2 stop bit
  UCSR0C |= (1 << USBS0);
}

unsigned long millis_target;

void loop() {
  // put your main code here, to run repeatedly:

  if ((UCSR0A & (1 << RXC0)))
  {
    char input = UDR0;
    String output = "Character received: ";

    for (uint8_t i = 0; i < output.length(); i++)
    {
      // Wait until the data transmit buffer is empty
      while (!(UCSR0A & (1 << UDRE0)));

      // Add new data to the transmit buffer
      UDR0 = output[i];
    }

    // Wait until the data transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Add new data to the transmit buffer
    UDR0 = input;

    // Wait until the data transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Add new data to the transmit buffer
    UDR0 = '\n';
  }

  if (millis() >= millis_target)
  {
    millis_target = millis_target + 5000;
    
    // Output data
    String output = "Hello World!\n";
    for (uint8_t i = 0; i < output.length(); i++)
    {
      // Wait until the data transmit buffer is empty
      while (!(UCSR0A & (1 << UDRE0)));

      // Add new data to the transmit buffer
      UDR0 = output[i];
    }
  }
}