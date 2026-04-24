#include <avr/io.h>

# define MYUBRR  ((F_CPU / (8L * UART_BAUDRATE)) - 1)

void    uart_init(void)
{
    UCSR0A |= (1 << U2X0);                  // Double Speed before sending MYUBRR value

    UBRR0 = (unsigned int)MYUBRR;

    UCSR0B = (1 << TXEN0) | (1 << RXEN0);   //Enable Receiver
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void    uart_tx(unsigned char c)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;                               //UART Data Register: print out the character
}

unsigned char    uart_rx(void)
{
    while (!(UCSR0A & (1 << RXC0)));        //RXC: Receive Complete; as long as no type-in, wait 
    return UDR0;                            //UART Data Register: the received character
}

int     main(void)
{
    uart_init();
    while (1)
    {
        unsigned char received = uart_rx();
        uart_tx(received);
    }
    return 0;
}
