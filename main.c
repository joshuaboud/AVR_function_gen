/* AVR Function Generator
 * Author: Josh Boudreau 2019
 * For ATmega328P: R2R DAC on PORTD, momentary button to GND on PC1, pot wiper from 0~5V on PC0.
 * 16MHz crystal/resonator with no clock prescaler
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define NUM_SHAPES 5
#define ADC_RESOLUTION 1024UL
#define TIMER1_RESOLUTION 65536UL
#define ADCchannel 0

enum {SINE, RAMP, REV_RAMP, TRI, SQU};
enum {UP, DOWN};

uint8_t sine[32] = {
  0, 6,
  12, 19,
  25, 31,
  37, 43,
  49, 54,
  60, 65,
  71, 76,
  81, 85,
  90, 94,
  98, 102,
  106, 109,
  112, 115,
  117, 120,
  122, 123,
  125, 126,
  126, 127
};

volatile uint8_t quadrant = 0;
volatile uint8_t sine_itr = 0;
volatile uint8_t direction = UP;
volatile uint8_t shape = SINE;
volatile uint32_t freq_setting;

ISR (TIMER1_OVF_vect){
  switch(shape){
  case SINE:
    switch(quadrant){
    case 0:
      PORTD = 128 + sine[sine_itr++];
      if(sine_itr >= 31) quadrant++;
      break;
    case 1:
      PORTD = 128 + sine[sine_itr--];
      if(sine_itr == 0) quadrant++;
      break;
    case 2:
      PORTD = 128 - sine[sine_itr++];
      if(sine_itr >= 31) quadrant++;
      break;
    case 3:
      PORTD = 128 - sine[sine_itr--];
      if(sine_itr == 0) quadrant = 0;
      break;
    }
    break;
  case RAMP:
    PORTD+=4;
    break;
  case REV_RAMP:
    PORTD-=4;
    break;
  case TRI:
    switch(direction){
    case UP:
      PORTD += 4;
      if(PORTD >= 248) direction = DOWN;
      break;
    case DOWN:
      PORTD -= 4;
      if(PORTD == 0) direction = UP;
      break;
    }
    break;
  default:
  case SQU:
    PORTD = (((sine_itr += 4)%128) > 63)? 255 : 0;
    break;
  }
  // calculate new TCNT1
  TCNT1 = 65535 - ADC;
}

void initADC(){
  // Select Vref=AVcc
  ADMUX |= 0; //(1 << REFS0);
  // set prescaler to 128 and enable ADC in auto trigger mode
  ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN) | (1 << ADATE);
  // trigger source to free running mode
  ADCSRB = 0;
  // kick off first conversion
  ADCSRA |= (1 << ADSC);
}

void initTimer(){
  // init timer1
  TCCR1A = 0x00;
  TCCR1B = (1 << CS10);  // Timer mode with no prescaler
  TIMSK1 = (1 << TOIE1);   // Enable timer1 overflow interrupt(TOIE1)
  TCNT1 = 0x0;
}

int main(){
  uint8_t db_cnt = 0;
  DDRC &= ~(1 << PC0) & ~(1 << PC1); // PC0, PC1 inputs
  PORTC = (1 << PC1); // pullup on PC1, PC0 high Z
  
  DDRD = 0xFF; // all outputs
  PORTD = 0x0;
  
  initADC(); // for analog frequency input
  
  initTimer(); // for DAC output on PORTD
  
  sei();
  
  while(1){
    // poll for button press
    if((PINC & (1 << PC1)) == 0){
      db_cnt++;
      if(db_cnt >= 4){
        shape = (shape + 1)%NUM_SHAPES;
        PORTD = 0;
        while((PINC & (1 << PC1)) == 0); // wait for release
        db_cnt = 0;
      }
    }else{
      db_cnt = 0;
    }
    _delay_ms(10);
  }
  return 0;
}
