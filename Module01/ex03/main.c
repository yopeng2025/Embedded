#include <avr/io.h>
#include <util/delay.h>

//Pressing button SW1 increments the duty cycle by 10 %.
//Pressing button SW2 decrements the duty cycle by 10 %

int main(void)
{
    DDRB |= (1 << PB1);

    TCCR1A |= (1 << WGM11) | (1 << COM1A1);              
    TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS12);

    int duty_cycle = 1;
    ICR1 = F_CPU / 256; 
    OCR1A = ICR1 * 0.1 * duty_cycle; 

    DDRD &= ~(1 << PD2) | (1 << PD4);
    PORTD |= (1 << PD2) | (1 << PD4);

    while (1)
    {
       if (!(PIND & (1 << PD2)))
       {
            _delay_ms(100);
            if (!(PIND & (1 << PD2)))
            {
                duty_cycle++;
                if (duty_cycle > 10)
                    duty_cycle = 1;
                OCR1A = ICR1 * 0.1 * duty_cycle;
                while (!(PIND & (1 << PD2)))     // Wait until button is released (avoid multiple triggers when holding the button)
                    ;
                _delay_ms(100);                  // Debounce after button release
            }
       }

       if (!(PIND & ( 1 << PD4)))
       {
            _delay_ms(100);
            if (!(PIND & (1 << PD4)))
            {
                duty_cycle--;
                if (duty_cycle < 1)
                    duty_cycle = 10;
                OCR1A = ICR1 / 10 * duty_cycle;
                while (!(PIND & (1 << PD4)))
                    ;
                _delay_ms(100);
            }
       }
    }

    while (1){}
}