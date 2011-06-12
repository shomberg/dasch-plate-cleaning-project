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

const int CLK = IO_C0;
const int SI = IO_C1;
  
int main()   
{
  OrangutanSerial::setBaudRate(USB_COMM, 115200);

  char s_buffer[41];//325
  int size = 41;

  for(int i=0; i<300; i++)
  {
    OrangutanDigital::setOutput(SI, HIGH);
	OrangutanDigital::setOutput(CLK, HIGH);
	OrangutanDigital::setOutput(CLK, LOW);
	delay_ms(1);
	OrangutanDigital::setOutput(SI, LOW);

    for(int j=0; j<8; j++)
	{
	  for(int k=0; k<8; k++)
	  {
	    OrangutanDigital::setOutput(CLK, HIGH);
		
		OrangutanAnalog::read(0);
	    int r = 5*j;

	    int x = OrangutanAnalog::read(0);
	    int a = x/1000;
	    x = x - 1000*a;
	    int b = x/100;
	    x = x - 100*b;
	    int c = x/10;
	    x = x - 10*c;
	    int d = x;

	    s_buffer[r] = a + '0';
	    s_buffer[r+1] = b + '0';
	    s_buffer[r+2] = c + '0';
	    s_buffer[r+3] = d + '0';

	    s_buffer[r+4] = '\t';

	    s_buffer[40] = '\n';
		s_buffer[39] = '\r';

		OrangutanDigital::setOutput(CLK, LOW);
		delay_ms(1);
	  }

	  OrangutanSerial::send(USB_COMM, s_buffer, size);

      while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
      {
	    OrangutanSerial::check();
      }

	  
    }
    OrangutanSerial::send(USB_COMM, "\n\r\n", 3);

    while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
	{
	     OrangutanSerial::check();
	}
    
	OrangutanDigital::setOutput(CLK, HIGH);
	OrangutanDigital::setOutput(CLK, LOW);
	delay_ms(1);

	OrangutanDigital::setOutput(CLK, HIGH);
	OrangutanDigital::setOutput(CLK, LOW);
	delay_ms(1);
  }
  
  return 0;
}  
