#include <pololu/orangutan>
#include <pololu/PololuQTRSensors.h>


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
  OrangutanSerial::setBaudRate(USB_COMM, 115200);

  delay_ms(5000);

  char r_buffer[32];
  char s_buffer[32];
  unsigned char size = 32;

  OrangutanLCD::clear();
  OrangutanLCD::print("send bits mcsp");

  s_buffer[0] = 'm';
  s_buffer[1] = 'c';
  s_buffer[2] = 's';
  s_buffer[3] = 'p';
  s_buffer[4] = '\r';
  s_buffer[5] = '\n';

  //s_buffer = "mcsp\r\n";


  OrangutanSerial::send(USB_COMM, "mcsp is rock\r\n", 14);

  while(1)
  {
    if(OrangutanSerial::sendBufferEmpty(USB_COMM))
	  OrangutanLCD::print(".");
	else OrangutanLCD::print("-");
	OrangutanSerial::check();
	delay_ms(100);
  }

  delay_ms(2000);
  
  OrangutanLCD::clear();
  
    

  /*while(1)
  {
    OrangutanSerial::receive(USB_COMM, r_buffer, size);
    OrangutanLCD::print(r_buffer[0]);
	OrangutanLCD::print(r_buffer[1]);
	OrangutanLCD::print(r_buffer[2]);
	OrangutanLCD::print(r_buffer[3]);

	delay_ms(1000);
	OrangutanLCD::clear();
  }*/



  return 0;
}  
