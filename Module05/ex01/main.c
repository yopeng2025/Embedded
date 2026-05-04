#include <avr/io.h>
#include <util/delay.h>

// Read the value by using the ADC peripheral (hexadecimal, 8-bit[0 255])

/*
ADC: A/D Converter(Analog-to-Digital Converter)
     10-bit (need two 8-bit + 8-bit registers); Microcontroller register 8-bit
RV1: Potentiometer (0V-5V)
LER: Light Dependent Resistor(R14)                    (resistance decreases as light increases)
NTC: Negative Temperature Coefficient Thermistor(R20) (resistance decreases as temperature rises)
*/

# define MYUBRR  ((F_CPU / (8L * UART_BAUDRATE)) - 1)

void    uart_init(void)
{
    UBRR0 = (unsigned int)MYUBRR;          
    UCSR0A |= (1 << U2X0);                 
    UCSR0B = (1 << TXEN0);                 
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void    adc_init(void)
{
    ADMUX = (1 << REFS0) | (1 << ADLAR);                                // REFS0: Reference voltage set to AVCC (p.257)
                                                                        // ADLAR: Left adjust to read 8-bit result from ADCH (p.259)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // ADEN:  ADC Enable  (p.258)
                                                                        // ADPS:  ADC Prescaler Select Bits - Prescaler 128
}

uint8_t   adc_read(uint8_t channel)
{
    ADMUX = (ADMUX & 0Xf0) | (channel & 0x0f);  // High 4 bit REFS1 REFS0 ADLAR -    (does not change)    p.622
                                                // Low  4 bit MUX3  MUX2  MUX1  MUX0 (change according to channel)
    ADCSRA |= (1 << ADSC);                      // Start a Conversion: in conversion->high; conversion complete->low
    while (ADCSRA & (1 << ADSC));               // wait till conversion complete
    return ADCH; 
}

void    uart_tx(char c)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void    print_hex(int n)
{
    char hex[] = "0123456789abcdef";
    uart_tx(hex[(n >> 4) & 0x0f]);              // 1111 1010 -> 0000 [1111]
    uart_tx(hex[n & 0x0f]);                     // 1111 1010 -> 1111 [1010]
}

int     main(void)
{
    uart_init();
    adc_init();
    while (1)
    {
        uint8_t RV1 = adc_read(0);              // PC0 - ADC0 
        uint8_t LDR = adc_read(1);              // PC1 - ADC1
        uint8_t NTC = adc_read(2);              // PC2 - ADC2

        print_hex(RV1);
        uart_tx(',');
        uart_tx(' ');
        print_hex(LDR);
        uart_tx(',');
        uart_tx(' ');
        print_hex(NTC);
        
        uart_tx('\n');
        uart_tx('\r');
        _delay_ms(20);
    }
    return 0;
}
