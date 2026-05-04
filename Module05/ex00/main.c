#include <avr/io.h>
#include <util/delay.h>

/*
ADC: A/D Converter(Analog-to-Digital Converter)
RV1: Potentiometer (0V-5V)
Read the value of the linear potentiometer RV1 using the ADC peripheral
*/

# define MYUBRR  ((F_CPU / (8L * UART_BAUDRATE)) - 1)

void    uart_init(void)
{
    // UBRR0H = (unsigned char)(MYUBRR >> 8); //higher 4 bits go to UBRR0H 0000 0000 0000 [0001] DS40002061B p.621
    // UBRR0L = (unsigned char)(MYUBRR);      // lower 8 bits go to UBRR0L 0000 0001 [1001 1011]
    UBRR0 = (unsigned int)MYUBRR;          
    UCSR0A |= (1 << U2X0);                 //Double speed
    UCSR0B = (1 << TXEN0);                 //enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);//frame format: 8N1 (8-bit/1-byte; No parity check; 1 stop bit(USBS0 = 0))
}

void    adc_init(void)
{
    // ADMUX: ADC Multiplexer Selection Register p.257 
    // REFS0: AVCC as reference (supply voltage pin for the A/D Converter)
    // ADC:   10-bit (need two 8-bit + 8-bit registers); Microcontroller register 8-bit
    // p.259  ADAR: ADC Left Adjust Result (push 10-bit to left [1111 1111, 11]00 0000 && takes the left 10-bit)
    // Result: ADCH = [1111 1111] 1100 0000
    //         ADCL =  1111 1111 [11]00 0000 (abandon)
    ADMUX = (1 << REFS0) | (1 << ADLAR);     

    // ADCSRA: ADC Control and Status Register A p.258 
    // ADEN:   ADC Enable  
    // ADPS:   ADC Prescaler Select Bits - Prescaler 128                         
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // ADC Clock = F_CPU / Prescaler
    //                    16        = 1000kHz (too fast)
    //                    64        = 250 kHz (fast, affect accuracy because of insufficient charge time)
    //                    128       = 125 kHz (guarantee accuracy, between 50k-200k)
    //                    256       = 62.5kHz (slow, affect accuracy because of leakage)
}

void    uart_tx(char c)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void    print_hex(int n)
{
    char hex[] = "0123456789abcdef";
    uart_tx(hex[(n >> 4) & 0x0f]);      // 1111 1010 -> 0000 [1111]
    uart_tx(hex[n & 0x0f]);             // 1111 1010 -> 1111 [1010]
    uart_tx('\n');
    uart_tx('\r');
}

int     main(void)
{
    uart_init();
    adc_init();
    while (1)
    {
        ADCSRA |= (1 << ADSC);          // Starting a Conversion: in conversion->high; conversion complete->low
        while (ADCSRA & (1 << ADSC));   // wait till conversion complete
        uint8_t result = ADCH;
        print_hex(result);
        _delay_ms(20);
    }
    return 0;
}
