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

volatile int BUFFER_HOUR_SIZE = 8;
volatile int count = 0;
volatile int timer0_count = 0;
volatile int revolution_Time = 0;
volatile int next_Column = 0;
volatile int size_Column = 0;
volatile int hour = 0;
volatile int minute = 0;
volatile int mod = 0;



volatile int display_tab[120] = {
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000,
  0B00000000,0B00000000
};

void draw_time() {
  for (int i = 0; i < 120; i+=2) {
    display_tab[i] = 0B00000001;
  }
  for (int i = 1; i < 119; i+=2) {
    display_tab[i] = 0B00000000;
  }

  if (hour >= 12) {
    display_tab[120 - ((hour - 12)*2)*5] = 0B11110001;
    display_tab[120 - (((hour - 12)*2)*5) + 1] = 0B11111111;
  }
  else {
    display_tab[120 - (hour*2)*5] = 0B11110001;
    display_tab[120 - ((hour*2)*5) + 1] = 0B11111111;
  }

  display_tab[120 - minute*2] = 0B11111111;
  display_tab[120 - (minute*2) + 1] = 0B11111111;
}

void print_zero(int first_box) {
  display_tab[first_box] = 0B11111111;
  display_tab[first_box + 2] = 0B10000001;
  display_tab[first_box + 4] = 0B11111111;
}
void print_one(int first_box) {
  display_tab[first_box] = 0B11111111;
  display_tab[first_box + 2] = 0B00000010;
  display_tab[first_box + 4] = 0B00000100;
}
void print_two(int first_box) {
  display_tab[first_box] = 0B10011111;
  display_tab[first_box + 2] = 0B10010001;
  display_tab[first_box + 4] = 0B11110001;
}
void print_three(int first_box) {
  display_tab[first_box] = 0B11111111;
  display_tab[first_box + 2] = 0B10010001;
  display_tab[first_box + 4] = 0B10010001;
}
void print_four(int first_box) {
  display_tab[first_box] = 0B11111111;
  display_tab[first_box + 2] = 0B00010000;
  display_tab[first_box + 4] = 0B00011111;
}
void print_five(int first_box) {
  display_tab[first_box] = 0B11110001;
  display_tab[first_box + 2] = 0B10010001;
  display_tab[first_box + 4] = 0B10011111;
}
void print_six(int first_box) {
  display_tab[first_box] = 0B11110001;
  display_tab[first_box + 2] = 0B10010001;
  display_tab[first_box + 4] = 0B11111111;
}
void print_seven(int first_box) {
  display_tab[first_box] = 0B11111111;
  display_tab[first_box + 2] = 0B00000001;
  display_tab[first_box + 4] = 0B00000001;
}
void print_eight(int first_box) {
  display_tab[first_box] = 0B11111111;
  display_tab[first_box + 2] = 0B10010001;
  display_tab[first_box + 4] = 0B11111111;
}
void print_nine(int first_box) {
  display_tab[first_box] = 0B11111111;
  display_tab[first_box + 2] = 0B10010001;
  display_tab[first_box + 4] = 0B10011111;
}

void print_digit(int n, int pos) {
  switch (n){
    case 0: print_zero(pos);break;
    case 1: print_one(pos);break;
    case 2: print_two(pos);break;
    case 3: print_three(pos);break;
    case 4: print_four(pos);break;
    case 5: print_five(pos);break;
    case 6: print_six(pos);break;
    case 7: print_seven(pos);break;
    case 8: print_eight(pos);break;
    case 9: print_nine(pos);break;
    default:print_zero(pos);break;
  }
}

void print_separator(int first_box) {
  display_tab[first_box] = 0B10010000;
}

void print_hour() {
  for (int i = 0; i < 120; i++) {
    display_tab[i] = 0B00000000;
  }
  if (hour == 0) { 
    print_digit(0, 32);
    print_digit(0, 24);
  }
  else{
    print_digit(hour/10, 32);
    print_digit(hour%10, 24);
  }

  print_separator(18);

  if (minute == 0) {
    print_digit(0, 8);
    print_digit(0, 0);
  }
  else{
    print_digit(minute/10, 8);
    print_digit(minute%10, 0);
  }
}

void choose_mod() {
  if (mod == 1){
    print_hour();
  }
  else{
    draw_time();
  }
}

void USART_Init(unsigned int ubrr) {
  /*Set baud rate */
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  UCSR0A &= ~(1 << U2X0);
  /*Enable receiver and transmitter */
  UCSR0B = (1 << RXEN0)|(1 << TXEN0);
  /* Set frame format: 8data, 2stop bit */
  UCSR0C = (1 << USBS0)|(3 << UCSZ00);
  // UCSR0C = (1 << UMSEL00);
}


void USART_Transmit(char data) {
  /* Wait for empty transmit buffer */
  while ( !( UCSR0A & (1 << UDRE0)));
  /* Put data into buffer, sends the data */
  UDR0 = data;
}

void USART_Transmit_String(char *s) {
  int cp = 0;
  while(s[cp] != '\0'){
      USART_Transmit(s[cp]);
      cp += 1;
  }
}

unsigned char USART_Receive(void) {
  /* Wait for data to be received */
  while (!(UCSR0A & (1<<RXC0)));
  /* Get and return received data from buffer */
  return UDR0;
}

void USART_Receive_String(char *buffer) {
  int cpt = 0;
  do{
      buffer[cpt] = USART_Receive();
  } while (buffer[cpt++] != '\n');
}

void USART_Transmit_Hour() {
  char buffer[32];
  char bufferTmpHour[32];
  char bufferTmpMinute[32];
  sprintf(bufferTmpHour,"%d", hour);
  sprintf(bufferTmpMinute,"%d", minute);

  if (hour == 0 && minute < 10){
    buffer[0] = '0';
    buffer[1] = '0';
    buffer[2] = '0';
    buffer[3] = bufferTmpMinute[0];
  }
  else if (hour == 0 && minute >= 10){
    buffer[0] = '0';
    buffer[1] = '0';
    buffer[2] = bufferTmpMinute[0];
    buffer[3] = bufferTmpMinute[1];
  }
  else if (hour < 10 && minute < 10){
    buffer[0] = '0';
    buffer[1] = bufferTmpHour[0];
    buffer[2] = '0';
    buffer[3] = bufferTmpMinute[0];
  }
  else if (hour < 10 && minute >= 10){
    buffer[0] = '0';
    buffer[1] = bufferTmpHour[0];
    buffer[2] = bufferTmpMinute[0];
    buffer[3] = bufferTmpMinute[1];
  }
  else if (hour >= 10 && minute < 10){
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
  char h_value[32];
  h_value[0] = buffer[0];
  h_value[1] = buffer[1];
  char m_value[32];
  m_value[0] = buffer[2];
  m_value[1] = buffer[3];
  size_t fullsize = strlen(h_value) + 1 + strlen(m_value) + 1 + strlen(inter) + 1 + strlen(end) + 1;
  char response[fullsize];
  strcat(response, h_value);
  strcat(response, inter);
  strcat(response, m_value);
  strcat(response, end);
  USART_Transmit_String(response);
}

void buffer_hour_increment(int cpt) {
  minute++;

  if (minute == 60){
    hour++;
    minute = 0;
  }

  if (hour == 24){
    hour = 0;
  }
}

void fill_hour_and_mod(char *buffer_hour){
  char buffer_tmp[5];
  buffer_tmp[0] = buffer_hour[4];
  mod = atoi(buffer_tmp);
  buffer_tmp[0] = buffer_hour[0];
  buffer_tmp[1] = buffer_hour[1];
  hour = atoi(buffer_tmp);
  buffer_tmp[0] = buffer_hour[2];
  buffer_tmp[1] = buffer_hour[3];
  minute = atoi(buffer_tmp);

}


void SPI_MasterInit(void) {
  DDRB = (1<<DDB3) | (1<<DDB5) | (1<<DDB2);
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<DDB5);
}

void SPI_MasterTransmit(char cData) {
  SPDR = cData;
  while(!(SPSR & (1 << SPIF)));
}

void led_init() {
  SPI_MasterInit();
  DDRB |= _BV(PB4);
  DDRC |= _BV(PC1);
  DDRC |= _BV(PC2);
  DDRB |= _BV(PB2);
}

void timer0_init() {
  TCCR0B = _BV(CS00) | _BV(CS02); //prescaler /1024
}

void timer0_interrupt() {
  TIMSK0 = (1 << TOIE0);
}

void timer1_init() {
  OCR1A = 0xFFFE;
  TCNT1 = 0;
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B = _BV(CS12); //prescaler /1024
}

void timer1_interrupt() {

    TIMSK1 = (1 << OCIE1A) | (1 << TOIE1);
}

void magnet_init() {
    //active la broche PD2 en mode input pour pouvoir lire l'état du capteur aimant

    DDRD &= ~(1 << PD2);

    PCICR |= (1 << PCIE2);
}

void magnet_interrupt(){
    EIMSK |= (1 << INT0);
}

void led_exec() {
        int value1 = display_tab[next_Column];
        int value2 = display_tab[next_Column + 1];
        SPI_MasterTransmit(value1);
        SPI_MasterTransmit(value2);
        PORTC |= _BV(PC2);
        PORTC &= _BV(PC2);
}

int timer0_calc() {
    int prescaler_timer0 = 1024;
    int size_register_timer0 = 255;
    return (FOSC / prescaler_timer0) / size_register_timer0;
}

ISR(TIMER0_OVF_vect) {
    count++;
    if(count == timer0_calc()){
        timer0_count++;
        count = 0;
        if(timer0_count == 60){
            count = count + 13; // reduce temporal gap
            timer0_count = 0;
        }
    }
}

ISR(TIMER1_OVF_vect) {

}

ISR(INT0_vect) { // interuption aimant
  revolution_Time = TCNT1;
  size_Column = revolution_Time / 60;
  TCNT1 = 0;
  next_Column = 0;
  OCR1A = size_Column - 1;
  char buffer1[32];
  sprintf(buffer1,"count = %d, hour = %d & minute = %d\n",count ,hour , minute);
  USART_Transmit_String(buffer1);
}

ISR(TIMER1_COMPA_vect) { // interruption comparaison
  led_exec();
  OCR1A = TCNT1 + size_Column - 1;
  next_Column = next_Column + 2;// update position du pointeur du tableau d'affichage
}

int main() {
  USART_Init(MYUBRR); //initialisation de l'USART
  magnet_init();
  timer0_init();
  timer1_init();
  led_init();
  char buffer_hour[BUFFER_HOUR_SIZE];
  char buffer_mod[32];
  bool modify = true;
  char buffer_tmp[40];
  _delay_ms(5000);
  sprintf(buffer_tmp, "h:XX min:XX mod:X 13501 13h50 mod 1\n");
  USART_Transmit_String(buffer_tmp);
  USART_Receive_String(buffer_hour);
  fill_hour_and_mod(buffer_hour);
  timer0_interrupt();
  timer1_interrupt();
  magnet_interrupt();
  sei();
  while(1) {

      if (timer0_count == 0 && modify) {
          choose_mod();
          buffer_hour_increment(timer0_count);
          modify = false;
      }

      if (timer0_count == 1 && !modify) {
          modify = true;
      }
  }
  free(buffer_mod);
  free(buffer_hour);
}
