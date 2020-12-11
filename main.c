#include <avr/io.h>
#include <util/delay.h>
<<<<<<< HEAD
#include <stddef.h>
=======
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
>>>>>>> 1a23619170bde6cfe2d1a5c9cf9106df238d993c


#define FOSC 13000000 // Clock Speed
#define BAUD 38400
#define MYUBRR FOSC/16/BAUD-1


void USART_Transmit(char data){
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)));
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

void USART_Transmit_String(char *s){
    int cp = 0;
    while(s[cp] != '\0'){
        USART_Transmit(s[cp]);
        cp += 1;
    }
}
unsigned char USART_Receive(void){
    /* Wait for data to be received */
    while (!(UCSR0A & (1<<RXC0)));
    /* Get and return received data from buffer */
    return UDR0;
}

void USART_Init(unsigned int ubrr){
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0A &= ~(1<<U2X0);
    /*Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

int main() {
    // Active et allume la broche PB5 (led)
    DDRD &= _BV(PD2);
    int value;
    USART_Init(MYUBRR);
    while(1){
        //USART_Transmit_String(" Nothing to see buds ");
        char receive = USART_Receive();
        //if (receive == ''){
        //  receive = 'm';
        //}
        //value = PIND;
        //char res = value;
        char send_back = receive;
        USART_Transmit(send_back);
        //_delay_ms(1);
        //USART_Transmit_String("\n");
    }
}
//avrdude -p m328p -c arduino -P COM7 -U flash:w:a.bin
