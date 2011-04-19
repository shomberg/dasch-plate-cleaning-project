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
  OrangutanLCD::initPrintf();
  
  OrangutanLCD::clear();
  OrangutanLCD::print("About to init");

  unsigned char qtr_pins[] = {0, 1, 2, 3, 4, 5};
  PololuQTRSensorsAnalog qtr(qtr_pins, 6); 
  qtr.init(qtr_pins, 6, 10, IO_C0);    

  OrangutanAnalog anlg;

  OrangutanLCD::clear();
  OrangutanLCD::print("waiting for top button");
  
  OrangutanLCD::clear();
  OrangutanLCD::print("calibrating");

  for (int i = 0; i < 250; i++)  
  {   
    qtr.calibrate();   
    delay(20);   
  }  
  
  OrangutanLCD::clear();
  OrangutanLCD::print("waiting for top button");

  while (1)
  {
    OrangutanLCD::clear();
    OrangutanLCD::print("getting vals");
	delay_ms(1000);

    unsigned int sensor_values[6];
	unsigned int sensor_values2[6];
	for(int i= 0; i<6; i++)
	{
	  sensor_values2[i] = 7777;
	  sensor_values[i] = 5555;
	}

	unsigned char x = 0;



    qtr.emittersOff();
	qtr.read(sensor_values, x);
	qtr.read(sensor_values, x);

	for(int i = 0; i<6; i++)
	{
	  OrangutanLCD::clear();
	  OrangutanLCD::print("A: ");
	  OrangutanLCD::print(sensor_values[i]);

      anlg.read(i);
	  OrangutanLCD::print(" B: ");
	  OrangutanLCD::print(anlg.read(i));

	  delay_ms(2000);
	  OrangutanLCD::clear();
	}
	delay_ms(1000);
  }
  
  return 0;
}  
