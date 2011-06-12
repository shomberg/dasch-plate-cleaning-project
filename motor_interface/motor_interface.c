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
  while(1){

  OrangutanSerial::setBaudRate(USB_COMM, 115200);
  char r_buffer[6];
  r_buffer[1] = '\n';

  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 100, 10);

  OrangutanLCD::clear();
  char s[19] = "\n\n\n\n\nHello, Bob!\r\n";
  OrangutanSerial::send(USB_COMM, s, 19);
  OrangutanLCD::print("Hello, Bob!");

  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
	OrangutanSerial::check();
  }

  delay_ms(2000);
  





  while(OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000)==1)
  {
    OrangutanLCD::clear();
    OrangutanLCD::print("Choose Motor Direction: F = forward, B = backward");
    char s1[54] = "Choose Motor Direction: F = forward, B = backward\n\r\n";

    OrangutanSerial::send(USB_COMM, s1, 54);

    while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
    {
  	  OrangutanSerial::check();
    }

    delay_ms(2000);
  

	//OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  }
    char dir = ' '; 

	char a[11] = "Set to:\r\n";
    OrangutanSerial::send(USB_COMM, a, 10);
	while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
    {
	  OrangutanSerial::check();
    }
	OrangutanSerial::send(USB_COMM, r_buffer, 2);
	while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
    {
	  OrangutanSerial::check();
    }
	
    dir =  r_buffer[0];
  
  if(dir == 'F' || dir == 'f')
  {
    OrangutanDigital::setOutput(IO_C1, LOW);//set direction (DIR pin)
	OrangutanLCD::print("F");
  }
  else if(dir == 'B' || dir=='b')
  {
    OrangutanDigital::setOutput(IO_C1, HIGH);//set direction (DIR pin)
	OrangutanLCD::print("B");
  }
  else 
  {
    OrangutanLCD::print("ERR");
  }

  delay_ms(2000);







  OrangutanLCD::clear();
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 100, 10);
  OrangutanLCD::print("Input Motor Delay (ms): Integer 0002-1000");
  char s2[47] ="\nInput Motor Delay (ms): Integer 0002-1000\n\r\n";
  OrangutanSerial::send(USB_COMM, s2, 47);
  
  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
	OrangutanSerial::check();
  }

  int delay_time = 0; 
  char dt[5]; dt[4] = '\n';
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  dt[0] = r_buffer[0];
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  dt[1] = r_buffer[0];
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  dt[2] = r_buffer[0];
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  dt[3] = r_buffer[0];
  
  delay_time = (dt[0]-'0') * 1000 + (dt[1]-'0') * 100 + (dt[2]-'0') * 10 + (dt[3]-'0');
  
  OrangutanSerial::send(USB_COMM, a, 10);
  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
	OrangutanSerial::check();
  }
  OrangutanSerial::send(USB_COMM, dt, 4);
  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
	OrangutanSerial::check();
  }






  OrangutanLCD::clear();
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 100, 10);
  OrangutanLCD::print("Input Time (ms): Integer 0001-9999");
  char s3[40] = "\nInput Time (ms): Integer 0001-9999\n\r\n";
  OrangutanSerial::send(USB_COMM, s3, 40);
  
  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
	OrangutanSerial::check();
  }

  int run_time = 2000; // set to received value
  char rt[5]; rt[4] = '\n';

  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  rt[0] = r_buffer[0];
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  rt[1] = r_buffer[0];
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  rt[2] = r_buffer[0];
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  rt[3] = r_buffer[0];
  
  run_time = (rt[0]-'0') * 1000 + (rt[1]-'0') * 100 + (rt[2]-'0') * 10 + (rt[3]-'0');
  
  OrangutanSerial::send(USB_COMM, a, 10);
  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
	OrangutanSerial::check();
  }
  OrangutanSerial::send(USB_COMM, rt, 4);
  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
	OrangutanSerial::check();
  }







  for(int b=0; b<(run_time)/(2+delay_time); b++)
  {

	OrangutanDigital::setOutput(IO_C0, HIGH);
	delay_ms(2);

	OrangutanDigital::setOutput(IO_C0, LOW);
	delay_ms(delay_time);
  }



  }
  return 0;
}  

