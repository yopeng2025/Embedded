#include <avr/io.h>

// Use Timer1 to control the LED

int main(void)
{
    DDRB |= (1 << PB1); //output LED D2

    //Timer/Counter1 Control Register A: decide what to do with timer
    //COM1A0: Compare Output Mode                                
    //p.140, Table 16-1
    //Toggle OC1A（=PB1=D2）
    TCCR1A |= (1 << COM1A0);
    
    //Timer/Counter1 Control Register B
    //p.141, Table 16-4
    //CTC: Clear Timer on Compare Match (p.132 Table 16-6)
    //The timer will reset to 0 once it matches OCR1A
    TCCR1B |= (1 << WGM12);

    //Prescaler: Speed Adjuster (slow down F_CPU)
    //p.143 Table 16-5
    //change prescaler -> 256 clock cycle
    TCCR1B |= (1 << CS12);

    //Output Compare Register: set limit to timer/counter
    //Formula:     (F_CPU / (Prescaler * 2 * Target_Freq)) - 1
    //Calculation: (16,000,000 / (256 * 2 * 1Hz)) - 1 = 31249
    OCR1A = (F_CPU / 256) / 2 - 1;
    
    while (1) {}
}

/*
Timer1: 16-bit register (max: 65,535, only last for 0.005s in 16Mhz)
        therefore, lower Prescaler to 62,500Hz (16,000,000 / 256 clock cycle) per second
                                   == 31,250Hz per 0.5 second
                                   == 31,249Hz per 0.5 second (-1, because counter starts from 0)

Prescaler:
1          16M     Hz (overflow)
8           2M     Hz
64        250K     Hz
256      62.5K     Hz (PERFECT!)
1024   15.625K     Hz (loose precision)

*/