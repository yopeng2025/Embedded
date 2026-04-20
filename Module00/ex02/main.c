#include <avr/io.h>

//turn on LED D1 (PB0) when pressing SW1(PD2) button
int main(void)
{
    DDRB |= (1 << PB0);  //00000001 == DDRB(PB0) is 1 (OUTPUT)
    DDRD &= ~(1 << PD2); //00000001 -> 00000100 -> 11111011 -> xxxxx0xx == DDRD(PD2) is 0 (INPUT)
    PORTD |= (1 << PD2); //00000001 -> 00000100 -> PORTD(PD2) is 1(UP)
    while (1)
    {
        if (!(PIND & (1 << PD2))) //press button (PIND=0 & SW1(PD2)=1)=0 
            PORTB |= (1 << PB0);  //00000001 == PORTB(PB0) is 1(5V);
        else
            PORTB &= ~(1 << PB0); //00000001 -> 11111110 -> xxxxxxx0 == PORTB(PB0) is 0(0V)
    }
    return 0;
}

/*

DDR_X == 0 (INPUT) :
        PORT_X == PULL-UP resistor  0(GND) 1(UP)
DDR_X == 1 (OUTPUT) :
        PORT_X == SWITCH            0(0V) 1(5V)

PB0 == 0 
PD2 == 2 defined in header<>

~  : Bitwise NOT operator (~1 = 0)
&= : Bitwise AND operator (1&1=1 1&0=0)

PIN_X == Port Input Number X: Read the actual logic levels

*/
