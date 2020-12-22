#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include<avr/interrupt.h>
#include<stdio.h> 

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

void SPI_MasterInit(void){
    DDRB = (1<<DDB3) | (1<<DDB5) | (1<<DDB2);
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<DDB5);
}

void SPI_MasterTransmit(char cData) {
    SPDR = cData;
    while(!(SPSR & (1<<SPIF))) {
        
    }
}


int count = 0;

ISR(TIMER0_OVF_vect) {
    count++;
}

int main() {
    // Active et allume la broche PB5 (led)
    DDRD &= _BV(PD2); //active la broche PD2 en mode input pour pouvoir lire l'état du capteur aimant
    int value;
    USART_Init(MYUBRR); //initialisation de l'USART

    // On active le timer
    // Dans le registre TCCR0B, on met à 1 les bits CS00 et CS02
    // TCCR0B = (1<<CS00) | (1<<CS02);
    TCCR0B = _BV(CS00) | _BV(CS02); //On va chercer le bit value de CS00 et CS02
    // TCCR0B = 0B00000101;

    // on active l'interruption du timer, on modifie le registre TIMSK0. On peut activer les modes WGM pour gérer la limite du registre
    TIMSK0 = (1<<TOIE0);
    // TIMSK0 = 0B00000001;
    sei();
    

    // DDRD &= _BV(PD2);

    SPI_MasterInit();
    DDRB |= _BV(PB4);
    DDRC |= _BV(PC1);
    DDRC |= _BV(PC2);
    DDRB |= _BV(PB2); // faut le mettre à 0, ça peut merder (histoire master/slave)s
    int value1 = 0B00000001;
    int value2 = 0B00000000; 

    // USART_Init(MYUBRR);
    while(1){

        // MAGNET
        //USART_Transmit_String(" Nothing to see buds ");
        //char s = USART_Receive();
        value = PIND;
        char res = value + '0';
        char test = 'b';
        USART_Transmit(res);

        // value = PIND; //Aimant: recupérer la veleur du capteur aimant
        // char res = value + '0'; //transformation de l'int en char
        // char send_back = receive;
        // USART_Transmit(send_back);


        char buffer[32]; 

        sprintf(buffer,"counter = %d",count);

        USART_Transmit_String(buffer);
        // value = PIND;
        // char res = value + '0';
        // char test = 'b';
        // USART_Transmit(res);



        //Allumage de ses morts de la lumière
        SPI_MasterTransmit(value1);
        SPI_MasterTransmit(value2);
        PORTC |= _BV(PC2);
        PORTC &= _BV(PC2);
        _delay_ms(1000);
        SPI_MasterTransmit(value2);
        SPI_MasterTransmit(value2);
        PORTC |= _BV(PC2);
        PORTC &= _BV(PC2);
        _delay_ms(1000);


        //USART_Transmit_String("\n");
    }
}
//avrdude -p m328p -c arduino -P COM7 -U flash:w:a.bin
