#include <avr/io.h>

//1Hz == (on 0.5s & off 0.5s)

//volatile: prevent compiler from deleting the empty while loop
void    my_timer(volatile unsigned long loop)
{
    while (loop--)
        ;
}

int main(void)
{
    DDRB |= (1 << PB1);

    while(1)
    {
        //toggle PB1
        PORTB ^= (1 << PB1);   //bitwise XOR 1^1=0 0^1=1
        my_timer((F_CPU/11)/2);
    }
}

/*
CPU frequency: 
16MHz (MegaHertz) == 16,000,000 clock cycle / 1s
                      8,000,000 clock cycle / 0.5s

my_timer == 11 clock cycle 
assembly:
every loop in my_timer consumes 11 clock cycles  /  11 cycles per loop
.L6:
	movw r20,r24          1 clock cycles
	movw r22,r26          1  
	sbiw r24,1            2
	sbc r26,__zero_reg__  1
	sbc r27,__zero_reg__  1
	or r20,r21            1
	or r20,r22            1
	or r20,r23            1
	brne .L6              2

*/