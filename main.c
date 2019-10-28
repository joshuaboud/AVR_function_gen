#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define NUM_SHAPES 5
#define ADC_RESOLUTION 1024UL
#define TIMER1_RESOLUTION 65536UL

enum {SINE, RAMP, REV_RAMP, TRI, SQU};
enum {UP, DOWN};

uint8_t sine[128] = {
  0, 2, 3, 5, 6, 8, 9, 11,
  12, 14, 16, 17, 19, 20, 22, 23,
  25, 26, 28, 29, 31, 32, 34, 35,
  37, 38, 40, 41, 43, 44, 46, 47,
  49, 50, 51, 53, 54, 56, 57, 58,
  60, 61, 63, 64, 65, 67, 68, 69,
  71, 72, 73, 74, 76, 77, 78, 79,
  81, 82, 83, 84, 85, 86, 88, 89,
  90, 91, 92, 93, 94, 95, 96, 97,
  98, 99, 100, 101, 102, 103, 104, 105,
  106, 106, 107, 108, 109, 110, 111, 111,
  112, 113, 113, 114, 115, 115, 116, 117,
  117, 118, 118, 119, 120, 120, 121, 121,
  122, 122, 122, 123, 123, 124, 124, 124,
  125, 125, 125, 125, 126, 126, 126, 126,
  126, 127, 127, 127, 127, 127, 127, 127
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
      if(sine_itr >= 127) quadrant++;
      break;
    case 1:
      PORTD = 128 + sine[sine_itr--];
      if(sine_itr == 0) quadrant++;
      break;
    case 2:
      PORTD = 128 - sine[sine_itr++];
      if(sine_itr >= 127) quadrant++;
      break;
    case 3:
      PORTD = 128 - sine[sine_itr--];
      if(sine_itr == 0) quadrant = 0;
      break;
    }
    break;
  case RAMP:
    PORTD++;
    break;
  case REV_RAMP:
    PORTD--;
    break;
  case TRI:
    switch(direction){
    case UP:
      if(++PORTD == 255) direction = DOWN;
      break;
    case DOWN:
      if(--PORTD == 0) direction = UP;
      break;
    }
    break;
  default:
  case SQU:
    PORTD = (((++sine_itr)%128) > 63)? 255 : 0;
    break;
  }
  // calculate new TCNT1
  TCNT1 = freq_setting * (TIMER1_RESOLUTION - 2UL) / ADC_RESOLUTION;
}

ISR (ADC_vect){
  freq_setting = ADCL | (ADCH << 8);
  // kick off next conversion
  ADCSRA |= (1 << ADSC);
}

void initADC(){
  // Select Vref=AVcc
  ADMUX |= (1 << REFS0);
  //set prescaler to 128 and enable ADC
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN) | ( 1<< ADIE);
  // kick off first conversion
  ADCSRA |= (1 << ADSC);
}

void initTimer(){
  // init timer1
  TCCR1A = 0x00;
  TCCR1B = (1 << CS10);  // Timer mode with no prescler
  TIMSK1 = (1 << TOIE1);   // Enable timer1 overflow interrupt(TOIE1)
  TCNT1 = 0x0;
}

int main(){
  DDRC &= ~(1 << PC0) & ~(1 << PC1); // PC0, PC1 inputs
  PORTC = (1 << PC1); // pullup on PC1, PC0 high Z
  
  DDRD = 0xFF; // all outputs
  PORTD = 0x0;
  
  initADC(); // for analog frequency input
  
  initTimer(); // for DAC output on PORTD
  
  sei(); // Enable global interrupts by setting global interrupt enable bit in SREG
  
  while(1){
    // poll for button press
    if(PINB & (1 << PC1)){
      _delay_ms(20); // debounce
      if(PINB & (1 << PC1)){
        shape = (shape + 1)%NUM_SHAPES;
        while(PINB & (1 << PC1)); // wait for release
      }
    }
  }
  return 0;
}
