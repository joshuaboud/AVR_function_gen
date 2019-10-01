#include <stdio.h>

enum {SINE, RAMP, TRI, SQU};
#define NUM_SHAPES 4

unsigned char sine[128] = {
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

enum {UP, DOWN};

int main(){
  unsigned char OCR0A = 0;
  
  unsigned char i = 0;
  unsigned char shape = SINE;
  unsigned char quadrant = 0;
  unsigned char direction = UP;
  while(1){
    switch(shape){
    case SINE:
      switch(quadrant){
      case 0:
        OCR0A = 128 + sine[i++];
        if(i >= 127) quadrant++;
        break;
      case 1:
        OCR0A = 128 + sine[i--];
        if(i == 0) quadrant++;
        break;
      case 2:
        OCR0A = 128 - sine[i++];
        if(i >= 127) quadrant++;
        break;
      case 3:
        OCR0A = 128 - sine[i--];
        if(i == 0) quadrant = 0;
        break;
      }
      break;
    case RAMP:
      OCR0A = (++OCR0A)%256;
      break;
    case TRI:
      switch(direction){
      case UP:
        if(++OCR0A == 255) direction = DOWN;
        break;
      case DOWN:
        if(--OCR0A == 0) direction = UP;
        break;
      }
      break;
    case SQU:
      OCR0A = (((++i)%128) > 63)? 255 : 0;
      break;
    }
    
    // wait for 1/512th of period
    
    printf("%d\n",OCR0A);
  }
  return 0;
}
