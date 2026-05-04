#include <avr/io.h>
#include <avr/interrupt.h>

//Breathing LED: cycle(0%-100%-0%) in 1s
//Use Timer0(overflow interruption) to change Timer1(Duty Cycle)
//Every time Timer0 interrupts, CPU executes __vector_16

volatile int8_t up = 1;                             //1 going up(0-100); -1 going down(100-0)

void    __vector_16(void) __attribute__ ((signal, used, externally_visible));    //Timer 0 == vector_16 on ATmega328P

//Timer0 controls a full cycle  /\/\/\/\...
void    __vector_16(void)                           
{
    uint16_t    step = ICR1 / 30;                             // Timer0 overflows 61 times/s = 30 times/s
    if (up == 1)
    {
        if (OCR1A <= (ICR1 - step))                           // ICR1 == TOP
            OCR1A += step;
        else
        {
            OCR1A = ICR1;
            up = -1;
        }
    }
    else
    {
        if (OCR1A >= step)
            OCR1A -= step;
        else
        {
            OCR1A = 0;
            up = 1;
        }
    }
}

int main(void)
{
    DDRB |= (1 << PB1);                                     // LED D2(OC1A)

    //Timer1
    TCCR1A |= (1 << COM1A1) | (1 << WGM11);                 // non-inverting mode
    TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);    // Fast PWM mode (ICR1-TOP); Prescaler 8
    ICR1 = 2000;
    // ICR1 = (F_CPU / (Prescaler * PWM_F)) - 1
    // ICR1 = (F_CPU / (8L * 1000L)) - 1 = 1999             // p.132 (>60Hz none seen fricker) 
    OCR1A = 0;                                              // start at 0% duty cycle

    //Timer0: turn on overflow interrupt
    TCCR0B |= (1 << CS02) | (1 << CS00);                    // Prescaler 1024
    TIMSK0 |= (1 << TOIE0);                                 // Timer/Counter Overflow Interrupt Enable: enable overflow interrupt

    SREG = (1 << 7);
    while(1) {}
    return 0;
}

//Timer1: 16-bit timer; 
//        Fast PWM Mode(ICR1->TOP)
//        smooth transitions in brightness

//Timer0: 8-bit timer; max 255
//        overflows(&interrupt) every [16,000,000/(1024*256)≈61] times per second
//        perfect rate to update the LED brightness smoothly
