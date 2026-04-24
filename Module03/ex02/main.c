#include <avr/io.h>
#include <util/delay.h>

void    init_rgb(void)
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

int    main(void)
{
    init_rgb();
    uint8_t i = 0;
    while(1)
    {
        wheel(i++);
        _delay_ms(50);
    }
    return 0;
}
