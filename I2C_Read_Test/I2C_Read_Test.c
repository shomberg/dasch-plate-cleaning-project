#include <pololu/orangutan>
#include <i2cmaster.h>
#include <stdlib.h> 



#define PCF8575  0x40      // device address of PCF8575




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
  
int main()   
{
    
    unsigned char ret;
    
	OrangutanLCD::clear();
	OrangutanLCD::print("BEGIN ");
    //DDRB  = 0xff;                              // use all pins on port B for output 
    //PORTB = 0xff;                              // (active low LED's )
	
	delay_ms(1000);

    i2c_init();                                // init I2C interface
	
    OrangutanLCD::print("INIT ");
	
	/* write 0x75 to eeprom address 0x05 (Byte Write) */
    ret = i2c_start(PCF8575+I2C_READ);       // set device address and write mode
    
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
        u_byte0.byte0 = i2c_readAck();                       // write address = 5
        u_byte1.byte1 = i2c_readNak();                       // ret=0 -> Ok, ret=1 -> no ACK 
        i2c_stop();                            // set stop conditon = release bus 
    }
	OrangutanLCD::print("END");
	delay_ms(1000);

	OrangutanLCD::clear();
	OrangutanLCD::printBinary(u_byte0.byte0);
	OrangutanLCD::gotoXY(0,1);
	OrangutanLCD::printBinary(u_byte1.byte1);
    for(;;);	 
}
