#include <avr/io.h>
#include <util/delay.h>

# define MYUBRR  ((F_CPU / (8L * UART_BAUDRATE)) - 1)

void    init_uart(void)
{
    UBRR0 = (unsigned int)MYUBRR;           //DS40002061B p.182
    UCSR0A |= (1 << U2X0);                  //Double speed: reduce baud rate error percentage at high speeds
    UCSR0B = (1 << TXEN0)| (1 << RXEN0);    //enable Transmitter(send out) & Receiver
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //set frame format: 8N1 (8-bit/1-byte; No parity check; 1 stop bit(USBS0 = 0))
}

char    uart_rx(void)
{
    while(!(UCSR0A & (1 << RXC0)));         //RXC: Receive Complete
    return UDR0;                            //UART Data Register: print out the character
}

void    uart_tx(unsigned char c)
{
    while(!(UCSR0A & (1 << UDRE0)));        //UART Data Register Empty; 1:empty & ready to write; 0:still dealing with the last character; DS40002061B p.200
    UDR0 = c;                               
}

uint8_t     char_to_hex(char c)
{
    if (c >= '0' && c<= '9')
        return (c - '0');
    if (c >= 'a' && c <='f')
        return (c - 'a' + 10);
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    return 0;
}

    //h = 0x0A (0000 1010)  10
    //l = 0x0F (0000 1111)  15
    //0xAF     (1010 1111)  175 
uint8_t     hex_to_byte(char high, char low)
{
    uint8_t h = char_to_hex(high);
    uint8_t l = char_to_hex(low);

    return (h << 4 | l);       
}

void    init_rgb(void)
{
    DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
    
    //timer0 p.113
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);   //Fast PWM Mode(8-bit); Non-inverting Mode
    TCCR0B = (1 << CS01) | (1 << CS00);                                     //Prescaler 64   16MHz / 64 / 256 ≈ 976 Hz
    
    //timer2 p.162
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);                   //Fast PWM Mode; Non-inverting Mode
    TCCR2B = (1 << CS22);                                                   //Prescaler 64   16MHz / 64 / 256 ≈ 976 Hz
}

void    set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    //OCR(0-255)
    OCR0B = r;                                  //OC0B PD5 timer0
    OCR0A = g;                                  //OC0A PD6 timer0
    OCR2B = b;                                  //OC2B PD3 timer2
}

void wheel(uint8_t pos) 
{
    pos = 255 - pos;
    if (pos < 85)
        set_rgb(255 - pos * 3, 0, pos * 3);     //blue(0,0,255) -> red(255,0,0)   
    else if (pos < 170) 
    {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);     //green(0,255,0) -> blue(0,0,255) 
    } 
    else 
    {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);     //red(255,0,0) -> green(0,255,0)
    }
}

int    main(void)
{
    init_rgb();
    init_uart();

    char buffer[10];
    uint8_t i = 0;

    while(1)
    {
        char c = uart_rx();
        uart_tx(c);

        if (c == '\n' || c == '\r')
        {
            uart_tx('\n');
            uart_tx('\r');
            if (i > 0)
            {
                buffer[i] = '\0';

                if (i == 7 && buffer[0] == '#')
                {
                    //#RRGGBB
                    uint8_t r = hex_to_byte(buffer[1], buffer[2]);
                    uint8_t g = hex_to_byte(buffer[3], buffer[4]);
                    uint8_t b = hex_to_byte(buffer[5], buffer[6]);
                    set_rgb(r, g, b);
                }
                i = 0;
            }
        }
        else
        {
            if (c == '#' || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))
            {
                if (i < 9)
                {
                    buffer[i] = c;
                    i++;
                }
            }
        }
    }
    return 0;
}
