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
	OrangutanDigital::setInput(IO_D0, PULL_UP_ENABLED);
	OrangutanDigital::setInput(IO_D1, PULL_UP_ENABLED);
	OrangutanDigital::setInput(IO_D2, PULL_UP_ENABLED);
	OrangutanDigital::setInput(IO_D3, PULL_UP_ENABLED);

	while(1){
		OrangutanLCD::clear();
		OrangutanLCD::print("BUT ROT ROT SWIT");
		OrangutanLCD::gotoXY(0,1);
		if(OrangutanDigital::isInputHigh(IO_D0)){
			OrangutanLCD::print(1);
		}
		else{
			OrangutanLCD::print(0);
		}
		if(OrangutanDigital::isInputHigh(IO_D1)){
			OrangutanLCD::print(1);
		}
		else{
			OrangutanLCD::print(0);
		}
		if(OrangutanDigital::isInputHigh(IO_D2)){
			OrangutanLCD::print(1);
		}
		else{
			OrangutanLCD::print(0);
		}
		if(OrangutanDigital::isInputHigh(IO_D3)){
			OrangutanLCD::print(1);
		}
		else{
			OrangutanLCD::print(0);
		}
		delay_ms(5);
	}
}
