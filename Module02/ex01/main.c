#include <avr/io.h>
#include <avr/interrupt.h> //ISR

////Print 'Hello World!' every 2s

# define MYUBRR  ((F_CPU / (8L * UART_BAUDRATE)) - 1)

void    uart_init(void)
{
    UBRR0 = (unsigned int)MYUBRR;
    UCSR0A |= (1 << U2X0);
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void    uart_tz(unsigned char c)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void    uart_printstr(const char* str)
{
    while(*str)
    {
        uart_tz(*str);
        str++;
    }
}

void    timer1_init(void)
{
    TCCR1B |= (1 << WGM12);                 // Mode: CTC
    TCCR1B |= (1 << CS12) | (1 << CS10);    // Speed: Prescaler 1024 (p.142)
    OCR1A = (F_CPU / 1024) * 2 - 1;         // Set time (how many ticks in 1s) & print every 2s
    TIMSK1 |= (1 << OCIE1A);                // Action: times up(flag!) -> call ISR(TIMER1_COMPA_vect) (p.144-145)
}

// [Macro] ISR: Interrupt Service Routine
// [interrupt vector] TIMER1_COMPA_vect: Timer1(16-bit); CompareMatchA; Vector
// Function:
//          whever while(1) is being interrupted(reach set time, in this case), execute ISR
ISR(TIMER1_COMPA_vect)                      
{
    uart_printstr("Hello World!\r\n");      // \r: Return(cursor back to the head of line)
}

int     main(void)
{
    uart_init();
    timer1_init();

// [Macro] sei: Set Enable Interrupts
// set I-bit in SREG to 1 (then CPU responds to interupt request)
//p.212 
    sei();

    while (1) {}
    return 0;
}