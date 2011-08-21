#include <pololu/orangutan>
#include <i2cmaster.h>
#include <stdlib.h> 



#define PCA9555  0x44      // device address of PCA9555 chip




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

/*initialize first and second bytes, can be manipulated as a byte u_bytex.bytex or as different bits u_bytex.bits_in_bytex.bity*/
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
        i2c_write(0x0);									//command byte 0 input port 0, 1 input port 1, 2 output port 0,
														//3 output port 1, 4 polarity inversion 0, 5 pol. inversion 1,
		i2c_start(PCA9555+I2C_READ);												//6 config 0, 7 config 1 
		u_byte0.byte0 = i2c_readAck();                       // read first byte and send Ack, requesting more
        u_byte1.byte1 = i2c_readNak();                       // read second byte and send stop condition
        i2c_stop();                            // set stop conditon = release bus 
    }
	OrangutanLCD::print("END");
	delay_ms(1000);

	OrangutanLCD::clear();
	OrangutanLCD::printBinary(u_byte0.byte0);			//print first byte
	OrangutanLCD::gotoXY(0,1);							//newline
	OrangutanLCD::printBinary(u_byte1.byte1);			//print second byte
    for(;;);	 
}
