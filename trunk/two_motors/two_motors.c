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



int gcf(int in1, int in2, int in3) //int
{
	int gcf = 1;
	for(int i = 2; i <= 100*in1; i ++)
	{
		if(100*in1 % i == 0)
		{
			if(100*in2 % i == 0)
			{
				if(100*in3 % i == 0)
				{
					gcf = i;
				}
			}
		}
	}
	return gcf;
}

union {
	struct {
		unsigned int bit_7 : 1;
		unsigned int bit_6 : 1;
		unsigned int bit_5 : 1;
		unsigned int bit_4 : 1;
		unsigned int bit_3 : 1;
		unsigned int bit_2 : 1;
		unsigned int bit_1 : 1;
		unsigned int bit_0 : 1;
	} bits_in_byte1;
	char byte1;
} u_byte1;

int main()
{
	int runtime = 8000, steplength = 2, totallength0 = 5, totallength1 = 6;
	//get inputs for runtime and steplength and totallength1 and totallength2\

OrangutanLCD::print("Hi");

u_byte1.byte1 = 0;  // initialize byte

u_byte1.bits_in_byte1.bit_6 = 1; // set bit 6

u_byte1.bits_in_byte1.bit_3 = 1; // set bit 3
OrangutanLCD::print(runtime);
//OrangutanLCD::print(steven);
OrangutanLCD::print( (int) u_byte1.byte1);	
	int delaytime = gcf(totallength0, totallength1, steplength)/100;
	for(int j = 0; j < (runtime)/(delaytime); j ++)
	{
		if(j % (totallength0)/(delaytime) < (steplength)/(delaytime))
		{
			OrangutanDigital::setOutput(IO_D0, HIGH);
		}
		else
		{
			OrangutanDigital::setOutput(IO_D0, LOW);
		}
		if(j % (totallength1)/(delaytime) < (steplength)/(delaytime))
		{
			OrangutanDigital::setOutput(IO_D1, HIGH);   //this is the same output!
		}
		else
		{
			OrangutanDigital::setOutput(IO_D1, LOW);
		}


		
		delay_ms(delaytime);
	}
}
