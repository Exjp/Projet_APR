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

volatile int BUFFER_HOUR_SIZE = 7;
volatile int count = 0;
volatile int timer0_count = 0;
volatile int timer1_count = 0;
volatile int revolution_Time = 0;
volatile int next_Column = 0;
volatile int size_Column = 0;

volatile int diplay_Tab[120] = {
    0B00000000,0B00000001,      // D
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,      // L
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,      // R
    0B00001110,0B00000001,
    0B00010001,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,      // 0
    0B00010000,0B00000001,
    0B00010000,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,      // W
    0B00010111,0B00000001,
    0B00001101,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,
    0B00011111,0B00000001,
    0B00010001,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,      // 0
    0B00011111,0B00000001,
    0B00011110,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,      // L
    0B00011111,0B00000001,
    0B00010001,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,      // L
    0B00010000,0B00000001,
    0B00010000,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,      // E
    0B00010000,0B00000001,
    0B00010000,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,      // H
    0B00010101,0B00000001,
    0B00010101,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,
    0B00011111,0B00000001,
    0B00000100,0B00000001,
    0B00011111,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
    0B00000000,0B00000001,
};
//
//
int hour = 0;
int *addr_hour = &hour;
int minute = 0;
int *addr_minute = &minute;

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

void USART_Receive_String(char **buffer){
    int cpt = 0;
    do{
        (*buffer)[cpt] = USART_Receive();
    } while ((*buffer)[cpt++] != '\n');
}

void USART_Transmit_Hour(){
    char buffer[32];
    char bufferTmpHour[32];
    char bufferTmpMinute[32];
    sprintf(bufferTmpHour,"%d",*addr_hour);
    sprintf(bufferTmpMinute,"%d",*addr_minute);

    if ((*addr_hour) == 0 && (*addr_minute) < 10){
      buffer[0] = '0';
      buffer[1] = '0';
      buffer[2] = '0';
      buffer[3] = bufferTmpMinute[0];
    }
    else if ((*addr_hour) == 0 && (*addr_minute) >= 10){
      buffer[0] = '0';
      buffer[1] = '0';
      buffer[2] = bufferTmpMinute[0];
      buffer[3] = bufferTmpMinute[1];
    }
    else if ((*addr_hour) < 10 && (*addr_minute) < 10){
      buffer[0] = '0';
      buffer[1] = bufferTmpHour[0];
      buffer[2] = '0';
      buffer[3] = bufferTmpMinute[0];
    }
    else if ((*addr_hour) < 10 && (*addr_minute) >= 10){
      buffer[0] = '0';
      buffer[1] = bufferTmpHour[0];
      buffer[2] = bufferTmpMinute[0];
      buffer[3] = bufferTmpMinute[1];
    }
    else if ((*addr_hour) >= 10 && (*addr_minute) < 10){
      buffer[0] = bufferTmpHour[0];
      buffer[1] = bufferTmpHour[1];
      buffer[2] = '0';
      buffer[3] = bufferTmpMinute[0];
    }
    else {
      buffer[0] = bufferTmpHour[0];
      buffer[1] = bufferTmpHour[1];
      buffer[2] = bufferTmpMinute[0];
      buffer[3] = bufferTmpMinute[1];
    }

    char *inter = " : ";
    char *end = " \n";
    char h_value[10];
    h_value[0] = buffer[0];
    h_value[1] = buffer[1];
    char m_value[10];
    m_value[0] = buffer[2];
    m_value[1] = buffer[3];
    size_t fullsize = strlen(h_value) + 1 + strlen(m_value) + 1 + strlen(inter) + 1 + strlen(end) + 1;
    char * response = (char *) malloc(fullsize);
    strcat(response, h_value);
    strcat(response, inter);
    strcat(response, m_value);
    strcat(response, end);
    USART_Transmit_String(response);
}

void buffer_hour_increment(int cpt){
  (*addr_minute)++;

  if ((*addr_minute) == 60){
    (*addr_hour)++;
    (*addr_minute) = 0;
  }

  if ((*addr_hour) == 24){
    (*addr_hour) = 0;
  }
}

void fill_hour(char* buffer){
  char buffer_tmp[5];
  buffer_tmp[0] = buffer[0];
  buffer_tmp[1] = buffer[1];
  *addr_hour = atoi(buffer_tmp);
  buffer_tmp[0] = buffer[2];
  buffer_tmp[1] = buffer[3];
  *addr_minute = atoi(buffer_tmp);
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

void led_init(){

    // Active et allume la broche PB5 (led)
    SPI_MasterInit();
    DDRB |= _BV(PB4);
    DDRC |= _BV(PC1);
    DDRC |= _BV(PC2);
    DDRB |= _BV(PB2); // faut le mettre à 0, ça peut merder (histoire master/slave)s
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
    // OCR1AL = 0B00011111;
    // OCR1AH = 0B00011110;
    OCR1A=0xFFFE;
    TCNT1 = 0;
    TCCR1A = 0;
    TCCR1B = 0;
    // /1 prescale + on init en plus OCR1A
    // TCCR1B = _BV(CS10);
    TCCR1B = _BV(CS12) ;
    // TIMSK1 = (1 << TOIE1);
    // USART_Transmit('p');
}

void timer1_interrupt(){

    TIMSK1 = (1<<OCIE1A) | (1 << TOIE1);
     // + init interrupt compare register OCR1A
}

void magnet_init(){
    //active la broche PD2 en mode input pour pouvoir lire l'état du capteur aimant

    DDRD &= ~(1 << PD2);

    PCICR |= (1 << PCIE2);
}

void magnet_interrupt(){
    EIMSK |= (1 << INT0);
}

void led_exec(){
        // int value1 = 0B00000001;
        // int value2 = 0B00000000;

        int value1 = diplay_Tab[next_Column];
        int value2 = diplay_Tab[next_Column + 1];
        SPI_MasterTransmit(value1);
        SPI_MasterTransmit(value2);
        PORTC |= _BV(PC2);
        PORTC &= _BV(PC2);


        // _delay_ms(1000);
        // SPI_MasterTransmit(value2);
        // SPI_MasterTransmit(value2);
        // PORTC |= _BV(PC2);
        // PORTC &= _BV(PC2);
}

void udapte_display() {
    for(int i = 0; i < 120; i = i + 2) {
        diplay_Tab[i] = 0;
        diplay_Tab[i+1] = 0;
    }

    // int pos_hour = ((hour / 60) * 3) - 1;
    // int pos_min = ((min / 60)) * 3) - 1;
//
    // diplay_Tab[pos_hour] = 1;
    // diplay_Tab[pos_min] = 1;
    // diplay_Tab[pos_min + 1] = 1;

}

int timer0_calc(){
    int prescaler_timer0 = 1024;
    int size_register_timer0 = 256;
    return (FOSC / prescaler_timer0) / size_register_timer0;
}

ISR(TIMER0_OVF_vect) {
    count++;
    if(count == timer0_calc()){
        timer0_count++;
        if(timer0_count == 60)
            timer0_count = 0;
        count = 0;
    }
}

ISR(TIMER1_OVF_vect) {

}

ISR(INT0_vect) { // interuption aimant
    // int tmp = TCNT1;

    revolution_Time = TCNT1;
    size_Column = revolution_Time / 60;
    TCNT1 = 0;
    timer1_count = 0;
    next_Column = 0;
    OCR1A = size_Column - 1;
    //char buffer1[32];

    // sprintf(buffer1,"timer0 = %d, hour = %d & minute = %d\n",timer0_count, hour, minute);

    //USART_Transmit_String(buffer1);
    // update_display();d
    // led_exec();
    // reset le tableau d'affichage des leds






}

ISR(TIMER1_COMPA_vect) { // interruption comparaison
    OCR1A = TCNT1 + size_Column - 1;
    next_Column = next_Column + 2;

    led_exec();
    // update position du pointeur du tableau d'affichage
}

int main() {
    USART_Init(MYUBRR); //initialisation de l'USART
    magnet_init();
    timer0_init();
    timer1_init();
    led_init();

    // _delay_ms(3000); // Laissez le temps au truc de se lancer


    // char buffer2[32];
    char *buffer_hour = (char*)malloc(BUFFER_HOUR_SIZE * sizeof(char));
    bool receive = false;
    bool modify = true;
    char trash[32];

    sei();
    while(1){


        // sprintf(buffer2,"hour = %d et minute = %d\n",hour, minute );

        // USART_Transmit_String(buffer2);

        if (!receive){

            USART_Receive_String(&buffer_hour);
            timer0_interrupt();
            timer1_interrupt();
            magnet_interrupt();
            fill_hour(buffer_hour);
            receive = true;
        }

        if (timer0_count == 0 && modify){
            buffer_hour_increment(timer0_count);
            USART_Transmit_Hour();
            modify = false;
        }
        if (timer0_count == 1 && !modify){
            modify = true;
        }
        // _delay_ms(500);
    }
    free(buffer_hour);
}
