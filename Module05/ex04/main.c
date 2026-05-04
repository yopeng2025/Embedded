#include <avr/io.h>
#include <util/delay.h>

// RV1 changes D5 color wheel & LED D1-D4(25% 50% 75% 100%)

void    rgb_init(void)
{
    DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
    
    //timer0 p.113 DS40002061B
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);   //Fast PWM Mode(8-bit); Non-inverting Mode
    TCCR0B = (1 << CS01) | (1 << CS00);                                     //Prescaler 64  PWM_Frequency:16MHz / 64 / 256 ≈ 976 Hz （<100Hz -> LED flicker）
    
    //timer2 p.162
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);                   //Fast PWM Mode; Non-inverting Mode
    TCCR2B = (1 << CS22);                                                   //Prescaler 64   16MHz / 64 / 256 ≈ 976 Hz
}

void    set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    //OCR(0-255)
    OCR0B = r;                                  //OC0B PD5 timer0
    OCR0A = g;                                  //OC0A PD6 timer0
    OCR2B = b;                                  //OC2B PD3 timer2
}

void wheel(uint8_t pos) 
{
    pos = 255 - pos;
    if (pos < 85)
        set_rgb(255 - pos * 3, 0, pos * 3);     //red(255,0,0) -> blue(0,0,255)  
    else if (pos < 170) 
    {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);     //blue(0,0,255) -> green(0,255,0)
    } 
    else 
    {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);     //green(0,255,0) -> red(255,0,0)
    }
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

int    main(void)
{
    rgb_init();
    adc_init();
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);

    while(1)
    {
        ADCSRA |= (1 << ADSC);          // Starting a Conversion: in conversion->high; conversion complete->low
        while (ADCSRA & (1 << ADSC));   // wait till conversion complete

        uint8_t result = ADCH;
        wheel(result);  
        
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4));    //turn off all LEDs
        if (result >= 60)
            PORTB |= (1 << PB0);
        if (result >= 128)
            PORTB |= (1 << PB1);
        if (result >= 192)
            PORTB |= (1 << PB2);
        if (result >= 253)
            PORTB |= (1 << PB4);
        _delay_ms(20);
    }
    return 0;
}

//AND 1&1=1 ...=0
