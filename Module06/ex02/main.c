#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdlib.h>    //dtostrf

# define MYUBRR  ((F_CPU / (8L * UART_BAUDRATE)) - 1)

void uart_init(void)
{
    UBRR0 = (unsigned int)MYUBRR;           //DS40002061B p.182
    UCSR0A |= (1 << U2X0);                  //Double speed: reduce baud rate error percentage at high speeds
    UCSR0B = (1 << TXEN0)| (1 << RXEN0);    //enable Transmitter(send out) & Receiver
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //set frame format: 8N1 (8-bit/1-byte; No parity check; 1 stop bit(USBS0 = 0))
}

void uart_tx(char c)
{
	while (!(UCSR0A & (1 << UDRE0))) ;   // wait till buffer is empty
	UDR0 = c;
}

void uart_tx_str(char* str)
{
    while(*str)
        uart_tx(*str++);
}

void i2c_init(void)
{
	TWBR = 72;								 // SCL_frequency = F_CPU / (16 + 2 * TWBR * Prescaler) = 100kHz (P.222)
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0));  
	TWCR = (1 << TWEN);						 
}

void i2c_start(void)								  
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));					             
}

void i2c_write(unsigned char data)
{
    TWDR = (uint8_t)data;                             
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));                   
}

void i2c_stop(void)									  
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); 
													  
	for (volatile int i = 0; i < 100; i++);			  
}

unsigned char i2c_read(int ack)
{
    if (ack)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);          
    else
        TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;    
}

float old_t[3];
float old_h[3];
int   data_count = 0;

void    update_old_data(float new_t, float new_h)
{
    old_t[2] = old_t[1];
    old_t[1] = old_t[0];
    old_t[0] = new_t;

    old_h[2] = old_h[1];
    old_h[1] = old_h[0];
    old_h[0] = new_h;

    if (data_count < 3)
        data_count++;
}

float average_data(float* array)
{
    float sum = 0;
    for (int i = 0; i < data_count; i++)
        sum += array[i];
    float average = sum / data_count;
    return average;
}

int main(void)
{
    uart_init();
    i2c_init();

    //power-on & calibrate
    _delay_ms(100);
    i2c_start();
    i2c_write((0x38 << 1) | 1);
    uint8_t status = i2c_read(0);
    i2c_stop();
    if (!(status & 0x08))                                   
    {
        i2c_start();
        i2c_write(0x38 << 1);
        i2c_write(0xBE);                                    
        i2c_write(0x08);
        i2c_write(0x00);
        i2c_stop();
        _delay_ms(10);                                      
    }

    while (1)
    {
        //trigger measurement
        i2c_start();						
		i2c_write(0x38 << 1);               
        i2c_write(0XAC);  
        i2c_write(0x33);                  
        i2c_write(0X00);
        i2c_stop();
        _delay_ms(80);

        //read raw data
        uint8_t raw[7];
        i2c_start();
        i2c_write((0x38 << 1) | 1);         
		for (int i = 0; i < 7; i++)        
        {
            if (i == 6)
                raw[i] = i2c_read(0);
            else
                raw[i] = i2c_read(1);
        }
        i2c_stop();
        //analyze raw data
        //AHT20-ADC： 20-bit 
        //raw[1]<<12: [0000 0000][0000 1111][1111]->[1111 1111][0000 0000][0000]
        //raw[2]<<4:  [0000 0000][0000 1111][1111]->[0000 0000][1111 1111][0000]
        //raw[3]>>4:  [0000 0000][0000 1111][0000]->[0000 0000][0000 0000][1111]
        //raw[3]<<16: [0000 0000][0000 0000][1111]->[1111 0000][0000 0000][0000]
        //raw[4]<<8:  [0000 0000][0000 1111][1111]->[0000 1111][1111 0000][0000]
        //uint16_t: max 1,048,575 (overflow)
        uint32_t raw_h = ((uint32_t)raw[1] << 12 | ((uint32_t)raw[2] << 4) | (raw[3] >> 4));
        uint32_t raw_t = ((uint32_t)(raw[3] & 0x0f) << 16) | ((uint32_t)raw[4] << 8) | raw[5];

        float show_h = ((float)raw_h / 1048576.0) * 100.0;       //AHT20 p.13
        float show_t = ((float)raw_t / 1048576.0) * 200.0 - 50.0;

        update_old_data(show_t, show_h);

        float average_h = average_data(old_h);
        float average_t = average_data(old_t);

        char h_str[15];
        char t_str[15];

        dtostrf(average_h, 4, 1, h_str); // Decimal To String Float
        dtostrf(average_t, 4, 1, t_str); // floating-point -> string (float, str_width, digit number after decimal point, str)
                                         // p.2 +-0.1

        uart_tx_str("Temperature: ");
        uart_tx_str(t_str);
        uart_tx_str(".C, Humidity: ");
        uart_tx_str(h_str);
        uart_tx_str("%\r\n");

        _delay_ms(2000);
    }
}