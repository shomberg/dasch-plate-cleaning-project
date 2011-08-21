#include <pololu/orangutan>
#include <i2cmaster.h>
#include <stdlib.h> 



#define PCA9555  0x40     // device address of PCA9555 chip




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

union u_byte0_tag{
	struct {
		unsigned int bit_0 : 1;
		unsigned int bit_1 : 1;
		unsigned int bit_2 : 1;
		unsigned int bit_3 : 1;
		unsigned int bit_4 : 1;
		unsigned int bit_5 : 1;
		unsigned int bit_6 : 1;
		unsigned int bit_7 : 1;
	} bits_in_byte0;
	char byte0;
} u_byte0;


union u_byte1_tag{
	struct {
		unsigned int bit_10 : 1;
		unsigned int bit_11 : 1;
		unsigned int bit_12 : 1;
		unsigned int bit_13 : 1;
		unsigned int bit_14 : 1;
		unsigned int bit_15 : 1;
		unsigned int bit_16 : 1;
		unsigned int bit_17 : 1;
	} bits_in_byte1;
	char byte1;
} u_byte1;



int gcf(int in1, int in2, int in3, int in4, int in5, int in6) //int
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
					if(100*in4 % i == 0)
					{
						if(100*in5 % i == 0)
						{
							if(100*in6 % i == 0)
							{
									gcf = i;
							}
						}
					}
				}
			}
		}
	}
	return gcf;
}
  
int main()   
{
    
    unsigned char ret;
	//int motorouts [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int runtime = 20000, steplength0 = 1, steplength1 = 2, steplength2 = 3, steplength3 = 4, steplength4 = 5;
	int totallength0 = 2, totallength1 = 3;
	int totallength2 = 4, totallength3 = 5, totallength4 = 6;
	int delaytime = 1;//gcf(totallength0, totallength1, totallength2, totallength3, totallength4, steplength)/100;
    
	u_byte0.byte0 = 0;  // initialize byte0
	u_byte1.byte1 = 0;  // initialize byte1
	
	OrangutanLCD::clear();
	OrangutanLCD::print("BEGIN ");
    //DDRB  = 0xff;                              // use all pins on port B for output 
    //PORTB = 0xff;                              // (active low LED's )
	
	delay_ms(1000);

    i2c_init();                                // init I2C interface
	
    OrangutanLCD::print("INIT ");
	
	/* write 0x75 to eeprom address 0x05 (Byte Write) */
    ret = i2c_start(PCA9555+I2C_WRITE);       // set device address and write mode
    
	OrangutanLCD::clear();
	OrangutanLCD::print("START ");
	
	if ( ret ) {
        /* failed to issue start condition, possibly no device found */
        
		OrangutanLCD::print("NACK ");
		i2c_stop();
        //PORTB=0x00;                            // activate all 8 LED to show error */
		OrangutanLCD::print("STOP ");
    }else {
		OrangutanLCD::print("ACK ");
        /* issuing start condition ok, device accessible */
		OrangutanLCD::print("BINIT ");
		i2c_write(0x6);
		i2c_write(0x0);
		i2c_write(0x0);
		i2c_stop();
		i2c_start(PCA9555+I2C_WRITE);
		u_byte0.bits_in_byte0.bit_1 = 1;
		u_byte0.bits_in_byte0.bit_2 = 1;
		u_byte0.bits_in_byte0.bit_4 = 1;
		u_byte0.bits_in_byte0.bit_5 = 1;
		u_byte0.bits_in_byte0.bit_7 = 1;
		u_byte1.bits_in_byte1.bit_10 = 1;
		u_byte1.bits_in_byte1.bit_12 = 1;
		u_byte1.bits_in_byte1.bit_13 = 1;
		u_byte1.bits_in_byte1.bit_15 = 1;
		u_byte1.bits_in_byte1.bit_16 = 1;

		delay_ms(1000);
		OrangutanLCD::clear();
		for(int j = 0; j < (runtime)/(delaytime); j ++)
		{

			//OrangutanLCD::print( (int) u_byte0.byte0);
			//OrangutanLCD::print(" ");

			if( (j % (totallength0/delaytime) ) < (steplength0/delaytime) )  //check if it is in the right period of the loop to send high
			{
				//motorouts[0] = 1;
				u_byte0.bits_in_byte0.bit_0 = 1; // set bit 0
			}
			else
			{
				//motorouts[0] = 0;
				u_byte0.bits_in_byte0.bit_0 = 0; // set bit 0
			}

			if( (j % (totallength1/delaytime) ) < (steplength1/delaytime) )
			{
				//motorouts[0] = 1;
				u_byte0.bits_in_byte0.bit_3 = 1; // set bit 1
			}
			else
			{
				//motorouts[0] = 0;
				u_byte0.bits_in_byte0.bit_3 = 0; // set bit 1
			}

			if( (j % (totallength2/delaytime) ) < (steplength2/delaytime) )
			{
				//motorouts[0] = 1;
				u_byte0.bits_in_byte0.bit_6 = 1; // set bit 1
			}
			else
			{
				//motorouts[0] = 0;
				u_byte0.bits_in_byte0.bit_6 = 0; // set bit 1
			}

			if( (j % (totallength3/delaytime) ) < (steplength3/delaytime) )
			{
				//motorouts[0] = 1;
				u_byte1.bits_in_byte1.bit_11 = 1; // set bit 1
			}
			else
			{
				//motorouts[0] = 0;
				u_byte1.bits_in_byte1.bit_11 = 0; // set bit 1
			}

			if( (j % (totallength4/delaytime) ) < (steplength4/delaytime) )
			{
				//motorouts[0] = 1;
				u_byte1.bits_in_byte1.bit_14 = 1; // set bit 1
			}
			else
			{
				//motorouts[0] = 0;
				u_byte1.bits_in_byte1.bit_14 = 0; // set bit 1
			}

			i2c_write(0x2);									// write command byte
			i2c_write(u_byte0.byte0);                       // write first byte of output
        	i2c_write(u_byte1.byte1);                       // write second byte of output
        	i2c_stop();                            // set stop conditon = release bus
			delay_ms(delaytime);
			i2c_start(PCA9555+I2C_WRITE);	//start up for next write
    	}
		u_byte0.bits_in_byte0.bit_2 = 0;
		u_byte0.bits_in_byte0.bit_5 = 0;
		u_byte1.bits_in_byte1.bit_10 = 0;
		u_byte1.bits_in_byte1.bit_13 = 0;
		u_byte1.bits_in_byte1.bit_16 = 0;
		i2c_write(0x2);
		i2c_write(u_byte0.byte0);
		i2c_write(u_byte1.byte1);
		i2c_stop();			//final stop
	}
	OrangutanLCD::print("END");
    for(;;);	 
}
