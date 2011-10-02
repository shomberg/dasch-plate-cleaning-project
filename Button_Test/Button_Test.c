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
	int button = 1, counterRefPush = 0, counterRefRel = 0, stateButton = 0, counter = 0;
	
	while(1){
		//button

		if(stateButton == 0 && OrangutanDigital::isInputHigh(IO_D0)){
			counterRefPush = counter;
			stateButton = 1;
		}
		if(stateButton == 1){
			if(counter - counterRefPush > 15){
				if(!OrangutanDigital::isInputHigh(IO_D0)){
					counterRefRel = counter;
					stateButton = 2;
				}
			}
			else if(!OrangutanDigital::isInputHigh(IO_D0)){
				stateButton = 0;
			}
		}
		if(stateButton == 2){
			if(OrangutanDigital::isInputHigh(IO_D0)){
				stateButton = 1;
				counterRefPush = counter;
			}
			else if(counter - counterRefRel > 15){
				button = 0;
				stateButton = 0;
			}
		}

		OrangutanLCD::gotoXY(0,0);
		if(OrangutanDigital::isInputHigh(IO_D0)){
			OrangutanLCD::print(1);
		}
		else{
			OrangutanLCD::print(0);
		}
		
		OrangutanLCD::print(stateButton);
		OrangutanLCD::print(button);
		OrangutanLCD::gotoXY(0,1);
		OrangutanLCD::print(counter);
		OrangutanLCD::print(" ");
		OrangutanLCD::print(counterRefPush);
		OrangutanLCD::print(" ");
		OrangutanLCD::print(counterRefRel);

		counter++;
		delay_ms(500);
	}
}
