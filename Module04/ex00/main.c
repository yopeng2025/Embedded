#include <avr/io.h>
#include <avr/interrupt.h>

//ATmega328P:   INT0 corresponds to __vector_1
//signal:       Ensures the compiler uses 'reti' and handles register pushing/popping
//used:         Prevents the compiler from optimizing out the function as "unused"
//externally_visible: Ensures the linker can see this symbol to populate the interrupt vector table correctly
void    __vector_1(void) __attribute__((signal, used, externally_visible));

void    __vector_1(void)
{
    PORTB ^= (1 << PB0);                                    //Toggle LED D1 (PB0)
    EIMSK &= ~(1 << INT0);                                  //disable INT0 interruption

    //Timer1: Debounce
    TCNT1 = 0;                                              //Timer&Counter: Reset the 16-bit Timer1 counter value to zero
    OCR1A = (F_CPU / 1024) / 2;                             //TOP:(16MHz / Prescaler) * 0.5s
    TIFR1 |= (1 << OCF1A) ;                                 //p.145Clear any pending Compare Match flag by writing a logical one
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);      //CTC mode; Prescaler 1024 
}

int main(void)
{
    DDRB |= (1 << PB0);                                     //LED     OUTPUT
    DDRD &= ~(1 << PD2);                                    //Button  INPUT
    PORTD |= (1 << PD2);                                    //Button  UP

    EICRA = (1 << ISC01);                                   //p.80External Interrupt Control Register(falling edge(Press button) of INT0 generates an interrupt request)
    EIMSK = (1 << INT0);                                    //P.81External Interrupt Mask Register: MCU->Interrupt Vector

    SREG = (1 << 7);                                        //set enable interruption
    
    while(1) 
    {
        if (TCCR1B & (1 << CS12))                           // Check if Timer1 is running (CS12 is set)
        {
            if (TIFR1 & (1 << OCF1A))                       // Check if 500ms has passed (Compare Match Flag)
            {
                TCCR1B = 0;                                 // Stop Timer1
                EIFR |= (1 << INTF0);                       // P.81Clear any interrupt flags accumulated during bouncing
                EIMSK |= (1 << INT0);                       // Re-enable INT0 for the next press
            }
        }
    }
    return 0;
}