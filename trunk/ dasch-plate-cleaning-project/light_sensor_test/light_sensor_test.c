#include <pololu/orangutan>


#include <stdlib.h> 

void * operator new(size_t size); 
void operator delete(void * ptr); 

void * operator new(size_t size) 
{ 
  return malloc(size); 
} 

void operator delete(void * ptr) 
{ 
  free(ptr); 
} 

__extension__ typedef int __guard __attribute__((mode (__DI__))); 

extern "C" int __cxa_guard_acquire(__guard *); 
extern "C" void __cxa_guard_release (__guard *); 
extern "C" void __cxa_guard_abort (__guard *); 

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);}; 
void __cxa_guard_release (__guard *g) {*(char *)g = 1;}; 
void __cxa_guard_abort (__guard *) {}; 

extern "C" void __cxa_pure_virtual(void); 
void __cxa_pure_virtual(void) {}; 

  
  
int main()   
{   
  OrangutanLCD lcd; 

  OrangutanAnalog anlg;

  OrangutanSerial::setBaudRate(USB_COMM, 115200);

  char s_buffer[6];
  unsigned char size = 6;

  for(int i=0; i<300; i++)
  {

	//lcd.clear();
    anlg.read(0);
	//lcd.print(anlg.read(0));

	delay_ms(100);

	int x = anlg.read(0);
	int a = x/1000;
	x = x - 1000*a;
	int b = x/100;
	x = x - 100*b;
	int c = x/10;
	x = x - 10*c;
	int d = x;

	//if((i%5)==0)
	  s_buffer[4] = '\r';
    //else s_buffer[4] = ' ';

	//if((i%5)==0)
	  s_buffer[5] = '\n';
	//else s_buffer[5] = '\t';

	s_buffer[0] = a + '0';
	s_buffer[1] = b + '0';
	s_buffer[2] = c + '0';
	s_buffer[3] = d + '0';


    OrangutanSerial::send(USB_COMM, s_buffer, size);

    while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
    {
	  OrangutanSerial::check();
    }
  }
  
  return 0;
}  
