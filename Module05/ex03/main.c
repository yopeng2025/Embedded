#include <avr/io.h>
#include <util/delay.h>

// Read the internal temperature sensor value & convert ADC raw value in celcius every 20ms

// ADC: A/D Converter(Analog-to-Digital Converter) Read the value by using the ADC peripheral

# define MYUBRR  ((F_CPU / (8L * UART_BAUDRATE)) - 1)

void    uart_init(void)
{
    UBRR0 = (unsigned int)MYUBRR;          
    UCSR0A |= (1 << U2X0);   
    UCSR0B = (1 << TXEN0);     
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void    adc_init_temp(void)
{
    //ADMUX: ADC Multiplexer Selection Register （p.257)
    //Temperature Measurement (p.256)
    //REFS:  Internal 1.1V Voltage Reference with external capacitor at AREF pin
    //MUX3:  1000 channel-ADC8 (Internal Temperature Sensor)
    ADMUX = (1 << REFS0) | (1 << REFS1) | (1 << MUX3);     
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
    adc_init_temp();
    _delay_ms(50);                       // Allow the 1.1V internal reference to stabilize

    while (1)
    {
        ADCSRA |= (1 << ADSC);           // Starting a Conversion: in conversion->high; conversion complete->low
        while (ADCSRA & (1 << ADSC));    // wait till conversion complete
        uint16_t result = ADC;
        //1.1V = 1100mV ; 25.C ≈ 314mV (p.256)
        //Typical Case: ADC = 314mV / 1100mV * 1024 ≈ 292
        //Due to process variation, temperature sensor output voltage varies from one chip to another
        //(p.256) offset = (ADC - temperature) / 1.1mV = 372 - 21 = 351
        uint16_t celsius = result - 351;  // Convert ADC raw value to Celsius using offset calibration
        print_decimal(celsius);
        uart_tx('\n');
        uart_tx('\r');
        _delay_ms(20);
    }
    return 0;
}
