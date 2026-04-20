#include <avr/io.h>
#include <util/delay.h>

//SW1 button (PD2) changes from the released state to the pressed state
int main(void)
{
    DDRB |= (1 << PB0);  //output
    DDRD &= ~(1 << PD2); //input
    PORTD |= (1 << PD2); //pull-up resistor: UP
    
    uint8_t last_state = 1; //UP
    while (1)
    {
        uint8_t current_state = (PIND & (1 << PD2)) >> PD2; // 0 or 1
        if (last_state == 1 && current_state == 0)
        {
            PORTB ^= (1 << PB0);   //Bitwise XOR 1^0=1 0^1=1 1^1=0 0^0=0
            _delay_ms(200);
        }
        last_state = current_state;
    }
    return 0;
}