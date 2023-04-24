#include <Arduino.h>

#define BAUD_RATE 9600
#define BIT_DURATION_MICROS (1000000UL / BAUD_RATE)
#define PACKET_START_BITS 1
#define PACKET_DATA_BITS 8
#define PACKET_PARITY_BITS 1
#define PACKET_END_BITS 1
#define PACKET_BITS (PACKET_START_BITS + PACKET_DATA_BITS + PACKET_PARITY_BITS + PACKET_END_BITS)

/*
Simple send method without the use of timers.
Blocks until the full message is sent.
*/
void send(uint16_t data)
{
    unsigned long micros_target = micros();

    // Send the start bits
    PORTD &= ~(1 << PORTD1);
    micros_target += BIT_DURATION_MICROS * PACKET_START_BITS;
    while (micros() < micros_target);

    // Send data bits
    uint8_t parity = 0;
    for (uint8_t i = 0; i < PACKET_DATA_BITS; i++)
    {
        uint16_t bit = (data >> i) & 1;

        PORTD = (PORTD & ~(1 << PORTD1)) | (bit << PORTD1);
        parity ^= bit;

        micros_target += BIT_DURATION_MICROS;
        while (micros() < micros_target);
    }

    // Send parity bits
    for (uint8_t i = 0; i < PACKET_PARITY_BITS; i++)
    {
        uint16_t bit = parity & 1;

        PORTD = (PORTD & ~(1 << PORTD1)) | (bit << PORTD1);
        parity ^= bit;

        micros_target += BIT_DURATION_MICROS;
        while (micros() < micros_target);
    }

    // Send end bits
    if (PACKET_END_BITS > 1)
    {
        PORTD &= ~(1 << PORTD1);
        micros_target += BIT_DURATION_MICROS * (PACKET_END_BITS - 1);
        while (micros() < micros_target);
    }

    PORTD |= (1 << PORTD1);
    if (PACKET_END_BITS)
    {
        micros_target += BIT_DURATION_MICROS;
        while (micros() < micros_target);
    }
}

/*
Simple print method without the use of timers.
Blocks until the full message is sent.
*/
void print(String text)
{
    for (unsigned int i = 0; i < text.length(); i++)
    {
        send(text.charAt(i));
    }
}

/*
Simple receive method without the use of interrupts or timers.
Blocks until a character is received.
*/
uint16_t receive()
{
    unsigned long micros_target;

    // Wait until start bit is received and start timing
    while (PIND & (1 << PIND0));
    micros_target = micros();

    // Wait until start bit is over
    micros_target += BIT_DURATION_MICROS * PACKET_START_BITS;
    while (micros() < micros_target);

    // Record data bits
    uint8_t parity = 0;
    uint16_t data = 0;
    for (uint8_t i = 0; i < PACKET_DATA_BITS; i++)
    {
        uint16_t bit = ((PIND & (1 << PIND0)) >> PIND0);

        data |= (bit << i);
        parity ^= bit;

        // Wait until this databit is over
        micros_target += BIT_DURATION_MICROS;
        while (micros() < micros_target);
    }

    // Record parity bits
    for (uint8_t i = 0; i < PACKET_PARITY_BITS; i++)
    {
        parity ^= ((PIND & (1 << PIND0)) >> PIND0);

        // Wait until this paritybit is over
        micros_target += BIT_DURATION_MICROS;
        while (micros() < micros_target);
    }

    // Wait until end bits are over
    micros_target += BIT_DURATION_MICROS * PACKET_END_BITS;
    while (micros() < micros_target);

    // Validate parity
    if (parity)
    {
        return 0UL;
    }

    return data;
}

void setup()
{
  //Set pin modes to input for power reduction.
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;
  
  //Enable pull-up on all pins for power reduction
    PORTB = 255;
    PORTC = 255;
    PORTD = 255;

    //Set up send pin (D1)
    DDRD |= (1 << DDD1);        //Output
    PORTD |= (1 << PORTD1);     //High

    //Set up receive pin (D0)
    DDRD &= ~(1 << DDD0);       //Input
    PORTD |= (1 << PORTD0);     //Enable internal pullup

    /*Setting up adc to read the analog input*/

    //Set voltage refference to AVcc
    ADMUX &= ~(1 << REFS1);
    ADMUX |= (1 << REFS0);
    
    //Left adjust the result
    ADMUX |= (1 << ADLAR);

    //Disable digital input for analog pins
    DIDR0 |= (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D) | (1 << ADC3D) | (1 << ADC4D) | (1 << ADC5D); 
}

void loop()
{
    //Assigning the received key
    char received = receive();

    //Start the terminal
    if(received != 'S')
    {
        return;
    }

    print("Commands:\n");
    print("Press D to print the digital inputs for pins 8 through 13.\n");
    print("Press A to print the anaolg inputs for pins 0 through 5.\n");
    print("Press C to clear the console.\n");

    while(true)
    {
        received = receive();
        //Printing the digital inputs for pins 8 through 13
        if(received == 'D')
        {
            print("Printing the digital inputs for pins 8 through 13\n");
            for (uint8_t i = 0; i < 6; i++)
            {
                print("DI");
                if(i < 2)
                {
                    print("0");
                    print((String)(i+8));
                }
                else
                {
                    print((String)(i+8));
                }
                print(": ");

                if((PINB & (1 << (0+i))))
                {
                    send('1');
                }
                else
                {
                    send('0');
                }
                print("\n");
            }
        }
        
        //Printing the analog inputs for pins 0 through 5
        if(received == 'A')
        {
            print("Printing the analog inputs for pins 0 through 5...\n");

            for (uint8_t i = 0; i < 6; i++)
            {
                if( i == 0)
                {
                    //Enable ADC
                    ADCSRA |= (1 << ADEN);
                }

                //Select ADC pin
                ADMUX &= ~(1 << MUX0) & ~(1 << MUX1) & ~(1 << MUX2) & ~(1 << MUX3);
                ADMUX |= i;

                //Start conversion and wait for completion
                ADCSRA |= (1 << ADSC);
                while (ADCSRA & (1 << ADSC));

                print("AI");
                print((String)i);
                print(": ");
                print((String)ADCH);
                print("\n");

                if(i == 5)
                {
                    // Disable ADC
                    ADCSRA &= ~(1 << ADEN);
                }
            }
        }

        //Clearing the console
        if(received == 'C')
        {
            print("Clearing the console...\n");
            delay(1000);
            //Clearing the console and adjusting the text to reset the cursor.
            print("\e[0H\e[0J");
            return;
        }
    }
}
