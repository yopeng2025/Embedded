#include <avr/io.h>
#include <util/delay.h>

//LED (D5) turns on in red, then green, then blue every 1s in a loop
//RED:   PD5
//GREEN: PD6
//BLUE:  PD3

int     main(void)
{
    DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD3);
    while (1) 
    {
        PORTD = (1 << PD5);
        _delay_ms(1000);

        PORTD = (1 << PD6);
        _delay_ms(1000);
    
        PORTD = (1 << PD3);
        _delay_ms(1000);
    }
    return 0;
}