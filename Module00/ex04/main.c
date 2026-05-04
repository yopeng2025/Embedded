#include <avr/io.h>
#include <util/delay.h>

//increments a value each time you press button SW1
//decrements a value each time you press button SW2

void    display_count(uint8_t count)
{
    uint8_t low_bit = count & 0x07;
    uint8_t high_bit = (count & 0x08) << 1;
    PORTB = high_bit | low_bit;
}

int main(void)
{
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4); //1 OUTPUT
    DDRD &= ~((1 << PD2) | (1 << PD4));                        //0 INPUT
    PORTD |= (1 << PD2) | ( 1 << PD4);                         //UP

    uint8_t count = 0;
    uint8_t last_sw1 = 1;
    uint8_t last_sw2 = 1;
    
    while (1)
    {
        uint8_t current_sw1 = (PIND & (1 << PD2)) >> PD2;
        uint8_t current_sw2 = (PIND & (1 << PD4)) >> PD4;

        if (last_sw1 == 1 && current_sw1 == 0)
        {
            count = (count + 1) & 0x0F;
            _delay_ms(100);
        }
        if (last_sw2 == 1 && current_sw2 == 0)
        {
            count = (count - 1) & 0x0F;
            _delay_ms(100);
        }
        display_count(count);
        last_sw1 = current_sw1;
        last_sw2 = current_sw2;
    }

    return 0;
}
