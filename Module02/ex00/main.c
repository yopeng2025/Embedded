#include <avr/io.h>
#include <util/delay.h>

//Print 'Z' every 1s

//DS40002061B p.182
//Calculation for U2X0 (Double Speed):
//8: divisor for Double Speed
//L(long): treat '8' as a Long (32-bit) integer
//int(avr-gcc) = 16-bit = 65,535 max
//8 * 115,200 = 921,600 (overflow!) 

/*
    MYUBRR = 16000000 / (8 * 115200) - 1 ≈ 16.36 → 16
    Real ≈ 16000000 / (8 * (16 + 1)) ≈ 117647
    Error ≈ +2.1% (should be < 2%)
*/
# define MYUBRR  ((F_CPU / (8L * UART_BAUDRATE)) - 1)

void    uart_init(void)
{
    // //DS40002061B p.185 set baud rate
    // //UBRR0: 12-bit register
    // //0000 0001 1001 1011
    // // ！！！errors will occur due to compiler optimization order or interrupt interference
    // UBRR0H = (unsigned char)(MYUBRR >> 8); //higher 4 bits go to UBRR0H 0000 0000 0000 [0001] DS40002061B p.621
    // UBRR0L = (unsigned char)(MYUBRR);      // lower 8 bits go to UBRR0L 0000 0001 [1001 1011]
    UBRR0 = (unsigned int)MYUBRR;          //safer way
    UCSR0A |= (1 << U2X0);                 //Double speed: reduce baud rate error percentage at high speeds
    UCSR0B = (1 << TXEN0);                 //enable transmitter(send out)
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);//set frame format: 8N1 (8-bit/1-byte; No parity check; 1 stop bit(USBS0 = 0))
    //==UCSR0C = (3 << UCSZ00);            //0000 0011 == 3; p.203
}

//DS40002061B p.186
void    uart_tx(unsigned char c)
{
    while(!(UCSR0A & (1 << UDRE0)));        //1:ready to write; 0:still dealing with the last character; DS40002061B p.200
    UDR0 = c;                               //assign character to UDR0 to start transmission
}

int     main(void)
{
    uart_init();
    while (1)
    {
        uart_tx('Z');
        _delay_ms(1000); //1Hz = 1s
    }
    return 0;
}

/*
USART: Universal Synchronous and Asynchronous serial Receiver and Transmitter
       transmits data one bit at a time using 
       [start bit] [data bits] [stop bit] at a fixed baud rate

UBRR: USART Baud Rate Register 
      (system transmits 115,200 bits of data per second)
      determines the serial communication speed 
      by dividing the system clock frequency 
      to match the desired baud rate

UCSR: USART Control and State Register
U2X0: USART Double Transmission Speed
UDRE: USART Data Register Empty
UDR0: USART I/O Data Register
UCSZ: USART Chracter SiZe p.203
      UCSZ02 UCSZ01 UCSZ00  character-size
        0       1      1       8-bit
 

Check Output:
    screen /dev/ttyUSB0 115200
Kill Screen:
    ctrl + a + k + y
    ctrl + a: Command Prefix (ctrl + c --> microcontroller)
    k:        Kill
    y:        Yes
*/