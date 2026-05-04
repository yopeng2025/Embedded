#include <avr/io.h>
#include <avr/interrupt.h>

# define MYUBRR  ((F_CPU / (8L * UART_BAUDRATE)) - 1)

void    uart_init(void)
{
    UBRR0 = (unsigned int)MYUBRR;
    UCSR0A |= (1 << U2X0);
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); //p.212 Receive Complete Interrupt Enable； trigger ISR
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void    uart_tx(unsigned char c)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;    
}

ISR(USART_RX_vect)
{
    unsigned char c = UDR0;
    if (c >= 'A' && c <= 'Z')
        c += 32;
    else if (c >= 'a' && c <= 'z')
        c -= 32;
    uart_tx(c);
}

int     main(void)
{
    uart_init();    
    SREG |= (1 << 7);       //set enable interruption p.20
    while(1){}
    return 0;
}