#include <avr/io.h>
#include <util/delay.h>

//RED:   PD5
//GREEN: PD6
//BLUE:  PD3

/*
name    R#   G#   B# 
red     ff   00   00
green   00   ff   00
blue    00   00   ff
yellow  ff   ff   00
cyan    00   ff   ff
magenta ff   00   ff
white   ff   ff   ff
*/

void    set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    if (r)
        PORTD |= (1 << PD5);
    else
        PORTD &= ~(1 << PD5);

    if (g)
        PORTD |= (1 << PD6);
    else
        PORTD &= ~(1 << PD6);

    if (b)
        PORTD |= (1 << PD3);
    else
        PORTD &= ~(1 << PD3);
}

int main(void)
{
    DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);

    while(1)
    {
        set_rgb(1, 0, 0); //RED
        _delay_ms(1000);

        set_rgb(0, 1, 0); //GREEN
        _delay_ms(1000);

        set_rgb(0, 0, 1); //BLUE
        _delay_ms(1000);

        set_rgb(1, 1, 0); //YELLOW
        _delay_ms(1000);

        set_rgb(0, 1, 1); //CYAN
        _delay_ms(1000);

        set_rgb(1, 0, 1); //MAGENTA
        _delay_ms(1000);

        set_rgb(1, 1, 1); //WHITE
        _delay_ms(1000);
    }
    return 0;
}