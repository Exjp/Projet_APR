#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define FOSC 13000000 // Clock Speed
#define BAUD 38400
#define MYUBRR FOSC/16/BAUD-1

int BUFFER_HOUR_SIZE = 7;

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

void USART_Receive_String(char *buffer){
    int cpt = -1;
    do{
        cpt++;
        buffer[cpt] = USART_Receive();
    } while (buffer[cpt] != '\0' && cpt < 32 - 1);
    buffer[cpt] = '\0';
}

void USART_Receive_Hour(char **buffer){
    int cpt = 0;
    do{
        (*buffer)[cpt] = USART_Receive();
    } while ((*buffer)[cpt++] != '\n');
}

void USART_Transmit_Hour(char *buffer){
    char *h = "Heure=";
    char *m = "Minute=";
    char h_value[4];
    h_value[0] = buffer[0];
    h_value[1] = buffer[1];
    char m_value[4];
    m_value[0] = buffer[3];
    m_value[1] = buffer[4];
    size_t fullsize = strlen(h) + 1 + strlen(m) + 1 + strlen(h_value) + 1 + strlen(m_value) + 1;
    char * response = (char *) malloc(fullsize);
    strcat(response, h);
    strcat(response, h_value);
    strcat(response, m);
    strcat(response, m_value);
    USART_Transmit_String(response);
}

//Copy str1 in str2
void copystr(char* str1, char* str2){
  int cpt = 0;
  while (str1[cpt] != '\0'){
    str2[cpt] = str1[cpt];
    cpt++;
  }
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
int tru_count = 0;

ISR(TIMER0_OVF_vect) {
    count++;
    if(count == 49){
        tru_count++;
        if(tru_count == 60)
            tru_count = 0;
        count = 0;
    }
}

ISR(TIMER1_OVF_vect) {
    tru_count++;
}

ISR(INT0_vect) { // interuption aimant
    tru_count++;
}

void led_init(){

    // Active et allume la broche PB5 (led)
    SPI_MasterInit();
    DDRB |= _BV(PB4);
    DDRC |= _BV(PC1);
    DDRC |= _BV(PC2);
    DDRB |= _BV(PB2); // faut le mettre à 0, ça peut merder (histoire master/slave)s


}

void led_exec(){
        int value1 = 0B00000001;
        int value2 = 0B00000000;

        SPI_MasterTransmit(value1);
        SPI_MasterTransmit(value2);
        PORTC |= _BV(PC2);
        PORTC &= _BV(PC2);
        _delay_ms(1000);
        SPI_MasterTransmit(value2);
        SPI_MasterTransmit(value2);
        PORTC |= _BV(PC2);
        PORTC &= _BV(PC2);
}

void timer0_init(){
    // On active le timer
    // Dans le registre TCCR0B, on met à 1 les bits CS00 et CS02
    // TCCR0B = (1<<CS00) | (1<<CS02);
    TCCR0B = _BV(CS00) | _BV(CS02); //On va chercer le bit value de CS00 et CS02
    // TCCR0B = 0B00000101;

}


void timer0_interrupt(){

    // on active l'interruption du timer, on modifie le registre TIMSK0. On peut activer les modes WGM pour gérer la limite du registre
    TIMSK0 = (1<<TOIE0);
    // TIMSK0 = 0B00000001;

    // TCCR0A = _BV(WGM00); //WGM
}
void timer1_init(){
    TCCR1B = _BV(CS10);
}

void timer1_interrupt(){

     TIMSK1 = (1<<TOIE1);
}

void magnet_init(){
    //active la broche PD2 en mode input pour pouvoir lire l'état du capteur aimant

    DDRD &= ~(1 << PD2);

    PCICR |= (1 << PCIE2);
}

void magnet_interrupt(){

    EIMSK |= (1 << INT0);




}

int main() {
    USART_Init(MYUBRR); //initialisation de l'USART
    // magnet_init();
    // timer0_init();
    timer1_init();
    // led_init();


    // timer_interrupt();
    // timer_init();

    // led_init();


    // magnet_interrupt();

    // timer0_interrupt();
    //timer1_interrupt();


    char buffer[32];
    char *buffer_hour = (char*)malloc(BUFFER_HOUR_SIZE * sizeof(char));
    char hour[2];
    char minute[2];



    sei();
    while(1){

        // MAGNET
        //USART_Transmit_String(" Nothing to see buds ");
        //char s = USART_Receive();


        // value = PIND; //Aimant: recupérer la veleur du capteur aimant
        // char res = value + '0'; //transformation de l'int en char
        // char send_back = receive;
        // USART_Transmit(send_back);


        //char buffer[32];

        //sprintf(buffer,"counter = %d\n",tru_count);

        //USART_Transmit_String(buffer);

        // led_exec();

        USART_Receive_Hour(&buffer_hour);
        USART_Transmit_Hour(buffer_hour);

        //_delay_ms(1000);
    }
    free(buffer_hour);
}
