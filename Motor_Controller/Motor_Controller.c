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
  //OrangutanLCD::print("do not step yet");
  //delay_ms(400);
  //OrangutanDigital::setOutput(IO_D0, LOW);
  //OrangutanDigital::setInput(IO_D1, HIGH_IMPEDANCE);

  //OrangutanLCD::clear();
  //OrangutanLCD::print("enable pin: ");
  //OrangutanLCD::print((int)(OrangutanDigital::isInputHigh(IO_D0)));
  //delay_ms(1000);
  //OrangutanLCD::clear();
  //OrangutanLCD::print((int)(OrangutanDigital::isInputHigh(IO_D0)));
  //delay_ms(400);

  //while(1)
  //{
    //OrangutanLCD::clear();
	//OrangutanLCD::print("step ");
    //OrangutanDigital::setOutput(IO_C1, HIGH);
	//delay_ms(1);
	
	
	//OrangutanLCD::print((int)(OrangutanDigital::isInputHigh(IO_D1)));


    //OrangutanLCD::clear();
    //OrangutanLCD::print("unstep ");
	//OrangutanDigital::setOutput(IO_C1, LOW);
	//delay_ms(1);
	
	//OrangutanLCD::print((int)(OrangutanDigital::isInputHigh(IO_D1)));

  //}

  OrangutanDigital::setOutput(IO_C1, LOW);//set direction (DIR pin)
  OrangutanLCD::print("Start wait. ");
  delay_ms(4000);
  OrangutanLCD::clear();
  OrangutanLCD::print("Start motor");

  //for(int i=1; i<10; i++)
  //{
    //for(int j=2; j<10; j++)
	//{
	  //OrangutanLCD::clear();
	  //OrangutanLCD::print("H: ");
	  //OrangutanLCD::print(i);
	  //OrangutanLCD::print(" L: ");
	  //OrangutanLCD::print(j);
      

	  for(int a=0; a<6400; a++)
	  {
	    //OrangutanPushbuttons::waitForButton(MIDDLE_BUTTON);

	    OrangutanDigital::setOutput(IO_C0, HIGH);
		delay_ms(2);

		OrangutanDigital::setOutput(IO_C0, LOW);
		delay_ms(2);
       }
	//}
  //}

  /*for(int i=0; i<400; i++)
  {
    OrangutanDigital::setOutput(IO_D0, HIGH);
	OrangutanLCD::clear();
	OrangutanLCD::print("step ");
	delay_ms(500);

	OrangutanDigital::setOutput(IO_D0, LOW);
	OrangutanLCD::clear();
	OrangutanLCD::print("low");
	delay_ms(500);
  }*/

  /*while(1)
  {
    if((OrangutanPushbuttons::isPressed(BOTTOM_BUTTON)) == (BOTTOM_BUTTON))
	{
	  OrangutanDigital::setOutput(IO_D0, LOW);
	  OrangutanLCD::clear();
	  OrangutanLCD::print("low");
	}
    else if((OrangutanPushbuttons::isPressed(TOP_BUTTON)) == (TOP_BUTTON))
	{
	  OrangutanDigital::setOutput(IO_D0, HIGH);
	  OrangutanLCD::clear();
	  OrangutanLCD::print("high");
	}
	else
	{
	  OrangutanLCD::clear();
	  OrangutanLCD::print("off");
	}

	delay_ms(400);
  }*/



  return 0;
}  
