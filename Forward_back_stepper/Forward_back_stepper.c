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


void sendGreeting(char* r_buffer)
{
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
}


int getNumSteps(char* r_buffer)
{
  OrangutanLCD::clear();
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 100, 10);
  OrangutanLCD::print("Input Num Steps: Integer 0001-1000");
  char s[40] ="\nInput Num Steps: Integer 0001-1000\n\r\n";
  OrangutanSerial::send(USB_COMM, s, 40);
  
  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
	OrangutanSerial::check();
  }

  int numsteps = 0; 
  char dt[5]; dt[4] = '\n';
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  dt[0] = r_buffer[0];
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  dt[1] = r_buffer[0];
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  dt[2] = r_buffer[0];
  OrangutanSerial::receiveBlocking(USB_COMM, r_buffer, 1, 10000);
  dt[3] = r_buffer[0];
  
  numsteps = (dt[0]-'0') * 1000 + (dt[1]-'0') * 100 + (dt[2]-'0') * 10 + (dt[3]-'0');

  char a[11] = "Set to:\r\n";
  OrangutanSerial::send(USB_COMM, a, 10);
  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
	OrangutanSerial::check();
  }
  OrangutanSerial::send(USB_COMM, dt, 5);
  while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
  {
    OrangutanSerial::check();
  }

  return numsteps;
}



int main()
{
  //OrangutanDigital::setOutput(IO_D0, LOW);  //set notSLEEP pin low, so sleep mode on

  //OrangutanSerial::setBaudRate(USB_COMM, 115200);
  //char r_buffer[6];
  //r_buffer[1] = '\n';

  //sendGreeting(r_buffer);
  //int numsteps = getNumSteps(r_buffer);
  int numsteps = 28210;


  OrangutanDigital::setOutput(IO_C1, LOW);   //set dir 'forward'
  OrangutanDigital::setOutput(IO_D0, HIGH);  //set notSLEEP pin high, so on normal running mode

  for(int i=0; i<numsteps; i++)
  {
	OrangutanDigital::setOutput(IO_C0, HIGH);
	delay_ms(.2);
	OrangutanDigital::setOutput(IO_C0, LOW);
	delay_ms(1);
  }

  delay_ms(200);

  OrangutanDigital::setOutput(IO_C1, HIGH); //set dir 'backward'

  for(int i=0; i<numsteps; i++)
  {
	OrangutanDigital::setOutput(IO_C0, HIGH);
	delay_ms(.2);
	OrangutanDigital::setOutput(IO_C0, LOW);
	delay_ms(1);
  }


  OrangutanDigital::setOutput(IO_D0, LOW);  //set notSLEEP pin low, so sleep mode on
}
