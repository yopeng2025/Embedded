#include <avr/io.h>

//Turn on LED D1（PB0）
int main(void)
{
    //DDR (Data Direction Register B) 0:input 1:output
    //PORT (Data Register B) 0:0V 1:5V
    //PB0 = 0 （Pin Mapping）PortB bit0 -> LED D1
    //<< shift: Move the number '1' to 0 bits to the left (still 0b00000001) 
    //|= bitwise OR
    DDRB |= (1 << PB0);
    PORTB |= (1 << PB0);
    while (1) {}
    return 0;
}
