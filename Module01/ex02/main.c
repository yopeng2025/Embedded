#include <avr/io.h>

// turns on and off LED D2 (PB1) at a frequency of 1Hz with a duty cycle of 10%
// on for 0.1s & off for 0.9s

int main(void)
{
    DDRB |= (1 << PB1);

    //DS40002061B p.142 Table 16-4
    //Mode14 Fast PWM(Pulse Width Modulation）(DS40002061B p.133 Table 16-7)
    //TCCR1A: WGM11 & WGM 10 (Compare Output Mode (COM))
    //TCCR1B: WGM13 & WGM 12 (Clock Select Prescaler (CS))
    //WGM:    Waveform Generation Mode
    TCCR1A |= (1 << WGM11);              
    TCCR1B |= (1 << WGM13) | (1 << WGM12);

    //DS40002061B p.140 Table 16-2
    //Clear OC1A on Compare Match, set OC1A at BOTTOM (non-inverting mode)
    //OC1A： Output Compare Channel A == LED D2(PB1)
    //(0-OCR1A)LED on; (OCR1A - ICR1) LED off
    TCCR1A |= (1 << COM1A1);

    //DS40002061B p.143 Table 16-5
    //Prescaler: 256 clock cycle 
    TCCR1B |= (1 << CS12);

    ICR1 = F_CPU / 256; //TOP: set PWM frequency (Input Capture Register 1)
    OCR1A = ICR1 * 0.1; //BOTTOM: control time for LED on

    while (1){}
}

//Fast PWM (Pulse Width Modulation）
//CTC: 50/50 on/off