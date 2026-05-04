#include <avr/io.h>
#include <util/delay.h>

//Read the value by using the ADC peripheral (decimal, 10-bit [1, 1023])

/*
ADC: A/D Converter(Analog-to-Digital Converter) 
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
    ADMUX = (1 << REFS0);                                               // REFS0: Reference voltage set to AVCC (p.257)
                                                                        // ADLAR=0(right adjust result) 10-bit value (0-1023)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // ADEN: ADC Enable  (p.258)
                                                                        // ADPS: ADC Prescaler Select Bits - Prescaler 128
}

void    uart_tx(char c)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

uint16_t   adc_read(uint8_t channel)
{
    ADMUX = (ADMUX & 0Xf0) | (channel & 0x0f);  // High 4 bit REFS1 REFS0 ADLAR -    (does not change)    p.622
                                                // Low  4 bit MUX3  MUX2  MUX1  MUX0 (change according to channel)
    ADCSRA |= (1 << ADSC);                      // Start a Conversion: in conversion->high; conversion complete->low
    while (ADCSRA & (1 << ADSC));               // wait till conversion complete
    return ADC;                                 // ADC = ADCH(8-bit) + ADCL(8-bit) = 16-bit(ADC: 10-bit (need two 8-bit + 8-bit registers))
}

void    print_decimal(uint16_t n)               // 10-bit = [0, 1023]
{
    if (n == 0)
    {
        uart_tx('0');
        return ;
    }
    int     i = 0;
    char    buffer[5];
    while (n > 0)
    {
        buffer[i] = (n % 10) + '0';
        n /= 10;
        i++;
    }
    while(i > 0)
    {
        i--;
        uart_tx(buffer[i]);
    }
}

int     main(void)
{
    uart_init();
    adc_init();
    while (1)
    {
        uint16_t RV1 = adc_read(0);              // PC0 - ADC0 
        uint16_t LDR = adc_read(1);              // PC1 - ADC1
        uint16_t NTC = adc_read(2);              // PC2 - ADC2

        print_decimal(RV1);
        uart_tx(',');
        uart_tx(' ');
        print_decimal(LDR);
        uart_tx(',');
        uart_tx(' ');
        print_decimal(NTC);
        
        uart_tx('\n');
        uart_tx('\r');
        _delay_ms(20);
    }
    return 0;
}
