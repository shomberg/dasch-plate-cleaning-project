#include <dasch.h>
#include <pololu/orangutan>
#include <i2cmaster.h>
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


  
void motor_and_write(int counter, int counterRef, int counterRefFive, int m1, int m2, int m3, int m4, int m5);


int main()   
{
    
    unsigned char ret1, ret2, ret3;
	int m2HalfPlate = 2000, m2WholePlate = 2000, m1LoadPlate = 2000, m2HomeFix = 2000;
	int m2Brush1Step = 2000, m2Brush2Step = 2000, m2Dry1Step = 2000, m2Dry2Step = 2000;
	int m2LoadBack = 2000, m2Dry1StepWhole = 2000, m2Brush2StepWhole = 2000;
    
	while(1){
	u_motorByte0.motorByte0 = 0;  // initialize motorByte0
	u_motorByte1.motorByte1 = 0;  // initialize motorByte1
	u_outputByte0.outputByte0 = 255;  // initialize outputByte0
	u_outputByte1.outputByte1 = 255;  // initialize outputByte1
	u_inputByte0.inputByte0 = 0;  // initialize inputByte0
	u_inputByte1.inputByte1 = 0;  // initialize inputByte1
	
	OrangutanLCD::clear();
	OrangutanLCD::print("BEGIN ");
	
	delay_ms(1000);

    i2c_init();                                // init I2C interface
	
    OrangutanLCD::print("INIT ");
	
    ret1 = i2c_start(I2C1+I2C_WRITE);       // set device address and write mode
	i2c_stop();
	ret2 = i2c_start(I2C2+I2C_WRITE);
	i2c_stop();
	ret3 = i2c_start(I2C3+I2C_WRITE);
	i2c_stop();
    
	OrangutanLCD::clear();
	OrangutanLCD::print("START ");
	
	if ( ret1 | ret2 | ret3 ) {
        /* failed to issue start condition, possibly no device found */
        
		OrangutanLCD::print("NAK ");
		i2c_stop();
		OrangutanLCD::print("STOP ");
    }
	else {
		OrangutanLCD::print("ACK ");
        /* issuing start condition ok, device accessible */
		OrangutanLCD::print("BINIT ");
		i2c_start(I2C1+I2C_WRITE);
		i2c_write(0x6);
		i2c_write(0x0);
		i2c_write(0x0);
		i2c_stop();
		
		i2c_start(I2C2+I2C_WRITE);
		i2c_write(0x6);
		i2c_write(0x0);
		i2c_write(0x0);
		i2c_stop();


		delay_ms(1000);
		OrangutanLCD::clear();


		
		int m1 = 0, m2 = 0, m3 = 0, m4 = 0, m5 = 0; 		//mx holds whether motor x should move: 0 = no, 1 = yes
		int state = 0;										//Holds program's current state
		int counter = 0;									//Counts iteration of the loop for timing purposes
		int counterRef = 0, counterRef14 = 0, counterRef26 = 0, counterRefFive = 0, counterRef30 = 0;
		int pWait = 100, mWait = 100, kWait = 100;
		int print0 = 1, print35 = 1;

		if(OrangutanDigital::isInputHigh(IO_D3))							//Tests if the switch is set to Maintenance Mode
		{
			OrangutanLCD::print("MAINTENANCE");
			delay_ms(1000);
			//initialize
			int button = 1, counterRefPush = 0, counterRefRel = 0, stateButton = 0;
			int print = 0;
			int mode = 3;

			OrangutanLCD::clear();
			OrangutanLCD::print("SELECT MODE");
			
			//Wait for button
			while(button != 0){
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
				counter++;
				delay_ms(1);
			}
			button = 1;
			counter = 0;
			OrangutanLCD::clear();

			if(OrangutanDigital::isInputHigh(IO_D1) && OrangutanDigital::isInputHigh(IO_D2)){
				state = S0;
				mode = 3;
				OrangutanLCD::clear();
				OrangutanLCD::print("INPUTS ");
				OrangutanLCD::gotoXY(0,1);
				print = 0;
			}
			
			while(state != -1){

				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
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

				
				if(!OrangutanDigital::isInputHigh(IO_D1) && !OrangutanDigital::isInputHigh(IO_D2) && mode != 0){
					state = S0;
					mode = 0;
					OrangutanLCD::clear();
					OrangutanLCD::print("INPUTS ");
					OrangutanLCD::gotoXY(0,1);
					print = 0;
				}
				if(OrangutanDigital::isInputHigh(IO_D1) && !OrangutanDigital::isInputHigh(IO_D2) && mode != 1){
					state = O0_ON;
					mode = 1;
					OrangutanLCD::clear();
					OrangutanLCD::print("OUTPUTS");
					OrangutanLCD::gotoXY(0,1);
					print = 0;
				}
				if(!OrangutanDigital::isInputHigh(IO_D1) && OrangutanDigital::isInputHigh(IO_D2) && mode != 2){
					state = M1_F;
					mode = 2;
					OrangutanLCD::clear();
					OrangutanLCD::print("MOTORS ");
					OrangutanLCD::gotoXY(0,1);
					print = 0;
				}
				if(OrangutanDigital::isInputHigh(IO_D1) && OrangutanDigital::isInputHigh(IO_D2) && mode != 3){
					state = S0;
					mode = 3;
					OrangutanLCD::clear();
					OrangutanLCD::print("INPUTS ");
					OrangutanLCD::gotoXY(0,1);
					print = 0;
				}

				//state conversions

				if(button == 0) {
					if(state <= S7){
						OrangutanLCD::gotoXY(0,1);
						button = 1;
						state ++;
						counterRef = counter;
						print = 0;
						if(state == S7 + 1){
							OrangutanLCD::clear();
							if(mode == 0){
								state = -1;
							}
							else{
								OrangutanLCD::print("OUTPUTS");
								OrangutanLCD::gotoXY(0,1);
							}
						}
					}
					else if(state <= O11_OFF){
						if(state % 2 == 0){
							OrangutanLCD::gotoXY(13,1);
							OrangutanLCD::print("OFF");
						}
						else{
							OrangutanLCD::gotoXY(0,1);
						}
						button = 1;
						state ++;
						counterRef = counter;
						print = 0;
						if(state == O11_OFF + 1){
							OrangutanLCD::clear();
							if(mode == 1){
								state = -1;
							}
							else{
								u_motorByte0.bits_in_motorByte0.m1Drop = 1;
								OrangutanLCD::print("MOTORS");
								OrangutanLCD::gotoXY(0,1);
							}
						}
					}
					else{
						button = 1;
						state ++;
						counterRef = counter;
						print = 0;
						if((state - 1) % 2 == 0){
							OrangutanLCD::gotoXY(11,1);
							OrangutanLCD::print("BACK ");
						}
						else{
							switch (state) {
								case M1_B + 1:
									OrangutanLCD::gotoXY(0,1);
									u_motorByte0.bits_in_motorByte0.m1Drop = 0;
									u_motorByte0.bits_in_motorByte0.m2Drop = 1;
								case M2_B + 1:
									OrangutanLCD::gotoXY(0,1);
									u_motorByte0.bits_in_motorByte0.m2Drop = 0;
									u_motorByte1.bits_in_motorByte1.m3Drop = 1;
								case M3_B + 1:
									OrangutanLCD::gotoXY(0,1);
									u_motorByte1.bits_in_motorByte1.m3Drop = 0;
									u_motorByte1.bits_in_motorByte1.m4Drop = 1;
								case M4_B + 1:
									OrangutanLCD::gotoXY(0,1);
									u_motorByte1.bits_in_motorByte1.m4Drop = 0;
									u_motorByte1.bits_in_motorByte1.m5Drop = 1;
								case M5_B + 1:
									OrangutanLCD::clear();
									u_motorByte1.bits_in_motorByte1.m5Drop = 0;
									state = -1;
									m5 = 0;
							}
						}
					}
				}
				
				/*if(state == 0 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 1;
					counterRef = counter;
					print = 0;
				}
				if(state == 1 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 2;
					counterRef = counter;
					print = 0;
				}
				if(state == 2 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 3;
					counterRef = counter;
					print = 0;
				}
				if(state == 3 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 4;
					counterRef = counter;
					print = 0;
				}
				if(state == 4 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 5;
					counterRef = counter;
					print = 0;
				}
				if(state == 5 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 6;
					counterRef = counter;
					print = 0;
				}
				if(state == 6 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 7;
					counterRef = counter;
					print = 0;
				}
				if(state == 7 && button == 0){
					OrangutanLCD::clear();
					button = 1;
					if(mode == 0){
						state = -1;
					}
					else{
						state = 8;
						OrangutanLCD::print("OUTPUTS");
						OrangutanLCD::gotoXY(0,1);
					}
					counterRef = counter;
					print = 0;
				}
				if(state == 8 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					state = 9;
					button = 1;
					counterRef = counter;
					print = 0;
				}
				if(state == 9 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 10;
					counterRef = counter;
					print = 0;
				}
				if(state == 10 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 11;
					counterRef = counter;
					print = 0;
				}
				if(state == 11 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 12;
					counterRef = counter;
					print = 0;
				}
				if(state == 12 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 13;
					counterRef = counter;
					print = 0;
				}
				if(state == 13 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 14;
					counterRef = counter;
					print = 0;
				}
				if(state == 14 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 15;
					counterRef = counter;
					print = 0;
				}
				if(state == 15 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 16;
					counterRef = counter;
					print = 0;
				}
				if(state == 16 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 17;
					counterRef = counter;
					print = 0;
				}
				if(state == 17 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 18;
					counterRef = counter;
					print = 0;
				}
				if(state == 18 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 19;
					counterRef = counter;
					print = 0;
				}
				if(state == 19 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 20;
					counterRef = counter;
					print = 0;
				}
				if(state == 20 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 21;
					counterRef = counter;
					print = 0;
				}
				if(state == 21 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 22;
					counterRef = counter;
					print = 0;
				}
				if(state == 22 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 23;
					counterRef = counter;
					print = 0;
				}
				if(state == 23 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 24;
					counterRef = counter;
					print = 0;
				}
				if(state == 24 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 25;
					counterRef = counter;
					print = 0;
				}
				if(state == 25 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 26;
					counterRef = counter;
					print = 0;
				}
				if(state == 26 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 27;
					counterRef = counter;
					print = 0;
				}
				if(state == 27 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 28;
					counterRef = counter;
					print = 0;
				}
				if(state == 28 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 29;
					counterRef = counter;
					print = 0;
				}
				if(state == 29 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					button = 1;
					state = 30;
					counterRef = counter;
					print = 0;
				}
				if(state == 30 && button == 0){
					OrangutanLCD::gotoXY(13,1);
					OrangutanLCD::print("OFF");
					button = 1;
					state = 31;
					counterRef = counter;
					print = 0;
				}
				if(state == 31 && button == 0){
					OrangutanLCD::clear();
					u_motorByte0.bits_in_motorByte0.m1Drop = 1;
					button = 1;
					if(mode == 1){
						state = -1;
					}
					else{
						state = 32;
						OrangutanLCD::print("MOTORS");
						OrangutanLCD::gotoXY(0,1);
					}
					counterRef = counter;
					print = 0;
				}
				if(state == 32 && button == 0){
					OrangutanLCD::gotoXY(11,1);
					OrangutanLCD::print("BACK ");
					button = 1;
					state = 33;
					counterRef = counter;
					print = 0;
				}
				if(state == 33 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					u_motorByte0.bits_in_motorByte0.m1Drop = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 1;
					button = 1;
					state = 34;
					counterRef = counter;
					print = 0;
				}
				if(state == 34 && button == 0){
					OrangutanLCD::gotoXY(11,1);
					OrangutanLCD::print("BACK ");
					button = 1;
					state = 35;
					counterRef = counter;
					print = 0;
				}
				if(state == 35 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					u_motorByte0.bits_in_motorByte0.m2Drop = 0;
					u_motorByte1.bits_in_motorByte1.m3Drop = 1;
					button = 1;
					state = 36;
					counterRef = counter;
					print = 0;
				}
				if(state == 36 && button == 0){
					OrangutanLCD::gotoXY(11,1);
					OrangutanLCD::print("BACK ");
					button = 1;
					state = 37;
					counterRef = counter;
					print = 0;
				}
				if(state == 37 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					u_motorByte1.bits_in_motorByte1.m3Drop = 0;
					u_motorByte1.bits_in_motorByte1.m4Drop = 1;
					button = 1;
					state = 38;
					counterRef = counter;
					print = 0;
				}
				if(state == 38 && button == 0){
					OrangutanLCD::gotoXY(11,1);
					OrangutanLCD::print("BACK ");
					button = 1;
					state = 39;
					counterRef = counter;
					print = 0;
				}
				if(state == 39 && button == 0){
					OrangutanLCD::gotoXY(0,1);
					u_motorByte1.bits_in_motorByte1.m4Drop = 0;
					u_motorByte1.bits_in_motorByte1.m5Drop = 1;
					button = 1;
					state = 40;
					counterRef = counter;
					print = 0;
				}
				if(state == 40 && button == 0){
					OrangutanLCD::gotoXY(11,1);
					OrangutanLCD::print("BACK ");
					button = 1;
					state = 41;
					counterRef = counter;
					print = 0;
				}
				if(state == 41 && button == 0){
					OrangutanLCD::clear();
					u_motorByte1.bits_in_motorByte1.m5Drop = 0;
					button = 1;
					state = -1;
					print = 0;
					m5 = 0;
				}*/

				//state actions
				switch (state) {
					case S0:
						if(print == 0){
							OrangutanLCD::print("SENSOR 0");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.plate);
					case S1:
						if(print == 0){
							OrangutanLCD::print("SENSOR 1");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureLift);
					case S2:
						if(print == 0){
							OrangutanLCD::print("SENSOR 2");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureHome);
					case S3:
						if(print == 0){
							OrangutanLCD::print("SENSOR 3");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixturePlate);
					case S4:
						if(print == 0){
							OrangutanLCD::print("SENSOR 4");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureBrush1);
					case S5:
						if(print == 0){
							OrangutanLCD::print("SENSOR 5");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureBrush2);
					case S6:
						if(print == 0){
							OrangutanLCD::print("SENSOR 6");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureDry1);
					case S7:
						if(print == 0){
							OrangutanLCD::print("SENSOR 7");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureDry2);
					case O0_ON:
						if(print == 0){
							OrangutanLCD::print("AC POWER     ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.ACPower = 0;
					case O0_OFF:
						u_outputByte0.bits_in_outputByte0.ACPower = 1;
					case O1_ON:
						if(print == 0){
							OrangutanLCD::print("BLOWER       ON  ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
					case O1_OFF:
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					case O2_ON:
						if(print == 0){
							OrangutanLCD::print("PLATE STOP   ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.plateStop = 0;
					case O2_OFF:
						u_outputByte0.bits_in_outputByte0.plateStop = 1;
					case O3_ON:
						if(print == 0){
							OrangutanLCD::print("PLATE RAISE  ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					case O3_OFF:
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						//u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					case O4_ON:
						if(print == 0){
							OrangutanLCD::print("PLATE LOWER  ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					case O4_OFF:
						u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					case O5_ON:
						if(print == 0){
							OrangutanLCD::print("BRUSH1 RAISE ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
						u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					case O5_OFF:
						u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
						//u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					case O6_ON:
						if(print == 0){
							OrangutanLCD::print("BRUSH1 LOWER ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
						u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
					case O6_OFF:
						u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					case O7_ON:
						if(print == 0){
							OrangutanLCD::print("BRUSH2 RAISE ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
						u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					case O7_OFF:
						u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
						//u_outputByte0.bits_in_outputByte0.brush2Lower = 0;
					case O8_ON:
						if(print == 0){
							OrangutanLCD::print("BRUSH2 LOWER ON ");
							print = 1;
						}
						u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
						u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
					case O8_OFF:
						u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					case O9_ON:
						if(print == 0){
							OrangutanLCD::print("PAPER RAISE  ON ");
							print = 1;
						}
						u_outputByte1.bits_in_outputByte1.ptRaise = 0;
						u_outputByte1.bits_in_outputByte1.ptLower = 1;
					case O9_OFF:
						u_outputByte1.bits_in_outputByte1.ptRaise = 1;
						//u_outputByte1.bits_in_outputByte1.ptLower = 0;
					case O10_ON:
						if(print == 0){
							OrangutanLCD::print("PAPER LOWER  ON ");
							print = 1;
						}
						u_outputByte1.bits_in_outputByte1.ptLower = 0;
						u_outputByte1.bits_in_outputByte1.ptRaise = 1;
					case O10_OFF:
						u_outputByte1.bits_in_outputByte1.ptLower = 1;
					case O11_ON:
						if(print == 0){
							OrangutanLCD::print("AIR KNIFE    ON ");
							print = 1;
						}
						u_outputByte1.bits_in_outputByte1.airKnife = 0;
					case O11_OFF:
						u_outputByte1.bits_in_outputByte1.airKnife = 1;
					case M1_F:
						if(print == 0){
							OrangutanLCD::print("LOADING    FRONT");
							print = 1;
						}
						u_motorByte0.bits_in_motorByte0.m1Dir = 0;
						m1 =  1;
					case M1_B:
						u_motorByte0.bits_in_motorByte0.m1Dir = 1;
						m1 =  1;
					case M2_F:
						if(print == 0){
							OrangutanLCD::print("DRIVE      FRONT");
							print = 1;
						}
						u_motorByte0.bits_in_motorByte0.m2Dir = 0;
						m2 =  1;
						m1 = 0;
					case M2_B:
						u_motorByte0.bits_in_motorByte0.m2Dir = 1;
						m2 =  1;
					case M3_F:
						if(print == 0){
							OrangutanLCD::print("BRUSH1     FRONT");
							print = 1;
						}
						u_motorByte0.bits_in_motorByte0.m3Dir = 0;
						m3 =  1;
						m2 = 0;
					case M3_B:
						u_motorByte0.bits_in_motorByte0.m3Dir = 1;
						m3 =  1;
					case M4_F:
						if(print == 0){
							OrangutanLCD::print("BRUSH2     FRONT");
							print = 1;
						}
						u_motorByte1.bits_in_motorByte1.m4Dir = 0;
						m4 =  1;
						m3 = 0;
					case M4_B:
						u_motorByte1.bits_in_motorByte1.m4Dir = 1;
						m4 =  1;
					case M5_F:
						if(print == 0){
							OrangutanLCD::print("PAPER      FRONT");
							print = 1;
						}
						u_motorByte1.bits_in_motorByte1.m5Dir = 0;
						m5 =  1;
						m4 = 0;
					case M5_B:
						u_motorByte1.bits_in_motorByte1.m5Dir = 1;
						m5 =  1;
				}

				//motor and write
				motor_and_write(counter, counterRef, counterRefFive, m1, m2, m3, m4, m5);



				/*if( ((counter - counterRef) % (totallength1) ) < (steplength1) && m1)  //check if it is in the right period of the loop to send high
				{
					u_motorByte0.bits_in_motorByte0.m1Step = 1; // set bit 0
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m1Step = 0; // set bit 0
				}

				if( ((counter - counterRef) % (totallength2) ) < (steplength2) && m2)
				{
					u_motorByte0.bits_in_motorByte0.m2Step = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m2Step = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength3) ) < (steplength3) && m3)
				{
					u_motorByte0.bits_in_motorByte0.m3Step = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m3Step = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength4) ) < (steplength4) && m4)
				{
					u_motorByte1.bits_in_motorByte1.m4Step = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.m4Step = 0; // set bit 1
				}

				if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && m5)
				{
					u_motorByte1.bits_in_motorByte1.m5Step = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.m5Step = 0; // set bit 1
				}

				i2c_start(I2C1+I2C_WRITE);
				i2c_write(0x2);									// write command byte
				i2c_write(u_motorByte0.motorByte0);                       // write first byte of output
       	 		i2c_write(u_motorByte1.motorByte1);                       // write second byte of output
       		 	i2c_stop();                            // set stop conditon = release bus

				i2c_start(I2C2+I2C_WRITE);
				i2c_write(0x2);
				i2c_write(u_outputByte0.outputByte0);
				i2c_write(u_outputByte1.outputByte1);*/

				counter ++;
				delay_ms(1);
			}
		}
		else{										//If switch is set to Run Mode
		
		OrangutanLCD::print("NORMAL");
		delay_ms(1000);

		int button = 1, counterRefPush = 0, counterRefRel = 0, stateButton = 0;
		
		OrangutanLCD::clear();
		OrangutanLCD::print("SELECT CYCLE");
		while(button != 0){
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
			counter++;
			delay_ms(1);
		}
		button = 1;
		counter = 0;
		OrangutanLCD::clear();
		OrangutanLCD::print("CYCLE:");

		//u_motorByte0.bits_in_motorByte0.m1Drop = 1;
		u_motorByte0.bits_in_motorByte0.m2Drop = 1;
		/*u_motorByte1.bits_in_motorByte1.m3Drop = 1;
		u_motorByte1.bits_in_motorByte1.m4Drop = 1;
		u_motorByte1.bits_in_motorByte1.m5Drop = 1;*/

		if(!OrangutanDigital::isInputHigh(IO_D1) && !OrangutanDigital::isInputHigh(IO_D2)){
			OrangutanLCD::gotoXY(7,0);
			OrangutanLCD::print("FIRST B");
			while(state != -1)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				if(state != 0){
					OrangutanLCD::gotoXY(0,1);
					OrangutanLCD::print("STATE ");
					OrangutanLCD::print(state);
				}

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

				//state conversions
	
				if(state == 0 && button == 0){
					button = 1;
					state = 1;
					counterRef = counter;
				}
				if(state == 1 && counter - counterRef > totallength1*m1LoadPlate/*&& u_inputByte0.bits_in_inputByte0.plate == 0*/){
					state = 2;
					counterRef = counter;
				}
				if(state == 2 && counter - counterRef > pWait){
					state = 3;
					counterRef = counter;
				}
				if(state == 3 && counter - counterRef > totallength2*m2HomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state = 4;
					counterRef = counter;
				}
				if(state == 4 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state = 5;
					counterRef = counter;
				}
				if(state == 5 && counter - counterRef > pWait){
						state = 6;
					counterRef = counter;
				}
				if(state == 6 && counter - counterRef > totallength2*m2Brush1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state = 7;
					counterRef = counter;
				}
				if(state == 7 && counter - counterRef > mWait){
					state = 8;
					counterRef = counter;
				}
				if(state == 8 && counter - counterRef > pWait){
					state = 9;
						counterRef = counter;
				}
					if(state == 9 && counter - counterRef > totallength2*m2HalfPlate){
					state = 10;
					counterRef = counter;
				}
				if(state == 10 && counter - counterRef > pWait){
					state = 11;
				}
				if(state == 11 && counter - counterRef > totallength2*m2HalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state = 12;
					counterRef = counter;
				}
				if(state == 12 && counter - counterRef > pWait){
					state = 13;
					counterRef = counter;
				}
				if(state == 13 && counter - counterRef > totallength2*m2HalfPlate){
					state = 14;
					counterRef14 = counter;
					}
				if(state == 14 && counter - counterRef > pWait){
					state = 23;
					counterRef = counter;
				}
				if(state == 23 && counter - counterRef > totallength2*m2Dry1StepWhole /*&& u_inputByte0.bits_in_inputByte0.fixtureDry1 == 0*/){
					state = 24;
					counterRef = counter;
				}
				if(state == 24 && counter - counterRef > kWait){
					state = 25;
					counterRef = counter;
				}
				if(state == 25 && counter - counterRef > totallength2*m2WholePlate){
					state = 26;
					counterRef26 = counter;
				}
				if(state == 26 && counter - counterRef > totallength2*m2Dry2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry2 == 0*/){
					state = 27;
					counterRefFive = counter;
				}
				if(state == 27 && counter - counterRefFive > mWait){
					state = 28;
					counterRef = counter;
				}
				if(state == 28 && counter - counterRef > pWait){
					state = 29;
					counterRef = counter;
				}
				if(state == 29 && counter - counterRef > totallength2*m2WholePlate){
					state = 30;
					counterRef30 = counter;
				}
				if(state == 30 && counter - counterRef > totallength2*m2LoadBack /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state = 31;
					counterRef = counter;
				}
					if(state == 31 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state = 32;
					counterRef = counter;
				}
				if(state == 32 && counter - counterRef > totallength2*m2HomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureHome == 0*/){
					state = 33;
					counterRef = counter;
				}
				if(state == 33 && counter - counterRef > pWait){
					state = 34;
					counterRef = counter;
				}
				if(state == 34 && counter - counterRef > totallength1*m1LoadPlate){
					state = 35;
					counterRef = counter;
				}
				if(state == 35 && counter - counterRef > 10){
					state = -1;
				}


				//state actions
						
				if(state == 0){
					if(u_inputByte0.bits_in_inputByte0.fixtureHome == 1){
						u_motorByte0.bits_in_motorByte0.m2Dir = 0;  //********* 0 is used as fixture backward ?cc?, 1 as forward ?c? **********
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						m2 = 1;
					}
					else{
						u_motorByte0.bits_in_motorByte0.m2Drop = 0;
						m2 = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						if(print0 == 1){
							OrangutanLCD::gotoXY(0,1);
							OrangutanLCD::print("PUSH TO START");
							print0 = 0;
						}
					}
				}
				if(state == 1){
					//OrangutanLCD::clear();
					u_outputByte0.bits_in_outputByte0.ACPower = 0;
					if((counter % 200) < 100){
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
					}
					else{
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					}
					m1 = 1;
					u_motorByte0.bits_in_motorByte0.m1Dir = 1;  //****** dir1 ******
					u_motorByte0.bits_in_motorByte0.m1Drop = 1;
					u_outputByte0.bits_in_outputByte0.plateStop = 0;
				}
				if(state == 2){
					u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.plateStop = 1;
						m1 = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 1;
					u_motorByte0.bits_in_motorByte0.m1Drop = 0;
				}	
				if(state == 3){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ****** dir2 *******
					m2 = 1;
				}
				if(state == 4){
						m2 = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
				}
				if(state == 5){
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 1;
				}
				//*********************************************
				if(state == 6){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 7){
					m2 = 0;
					u_motorByte1.bits_in_motorByte1.m3Drop = 1;
					u_motorByte0.bits_in_motorByte0.m3Dir = 0;  // ******* dir3 *******
					/*if(print6 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 1");
						print6 = 0;
					}*/
				}
				if(state == 8){
					m3 = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
				}
				if(state == 9){
					m2 = 1;
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;  // ******* dir2 *******
				}
				if(state == 10){
					m2 = 0;
					m3 = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
				}
				if(state == 11){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 12){
					m2 = 0;
					u_motorByte0.bits_in_motorByte0.m3Dir = 0;  // ******* dir3 *******
					m3 = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
				}
				if(state == 13){
					m2 = 1;
				}
				if(state == 14){
					m3 = 0;
					m2 = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
				}
				
				//*************************************************
				if(state == 23){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
					u_motorByte1.bits_in_motorByte1.m3Drop = 0;
				}
				if(state == 24){
					m2 = 0;
					u_outputByte1.bits_in_outputByte1.airKnife = 0;
						/*if(print24 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("DRY AIR");
						print24 = 0;
					}*/
				}
				if(state == 25){
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 26){
					u_outputByte1.bits_in_outputByte1.airKnife = 1;
				}
				if(state == 27){
					m2 = 0;
					u_motorByte1.bits_in_motorByte1.m5Drop = 1;
					u_motorByte1.bits_in_motorByte1.m5Dir = 0;
					m5 = 1;
				}
				if(state == 28){
					/*if(print28 == 1){
							OrangutanLCD::clear();
						OrangutanLCD::print("FINAL DRY");
						print28 = 0;
						}*/
					u_outputByte1.bits_in_outputByte1.ptRaise = 0;
				}
				if(state == 29){
					m2 = 1;
				}
				if(state == 30){
					m5 = 0;
						u_motorByte1.bits_in_motorByte1.m5Drop = 0;
					u_outputByte1.bits_in_outputByte1.ptRaise = 1;
					u_outputByte1.bits_in_outputByte1.ptLower = 0;
				}
				if(state == 31){
					m2 = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
				}
				if(state == 32){
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;
					m2 = 1;
				}
				if(state == 33){
					m2 = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 0;
					u_motorByte0.bits_in_motorByte0.m1Drop = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
				}
				if(state == 34){
					u_motorByte0.bits_in_motorByte0.m1Dir = 0;
					m1 = 1;
					}
				if(state == 35){
					m1 = 0;
					u_motorByte0.bits_in_motorByte0.m1Drop = 0;
					if(print35 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("END OF CYCLE");
						print35 = 0;
					}
				}





				if( ((counter - counterRef) % (totallength1) ) < (steplength1) && m1)  //check if it is in the right period of the loop to send high
				{
					u_motorByte0.bits_in_motorByte0.m1Step = 1; // set bit 0
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m1Step = 0; // set bit 0
				}

				if( ((counter - counterRef) % (totallength2) ) < (steplength2) && m2)
				{
					u_motorByte0.bits_in_motorByte0.m2Step = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m2Step = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength3) ) < (steplength3) && m3)
				{
					u_motorByte0.bits_in_motorByte0.m3Step = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m3Step = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength4) ) < (steplength4) && m4)
				{
					u_motorByte1.bits_in_motorByte1.m4Step = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.m4Step = 0; // set bit 1
				}

				if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && m5)
				{
					u_motorByte1.bits_in_motorByte1.m5Step = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.m5Step = 0; // set bit 1
				}

				i2c_start(I2C1+I2C_WRITE);
				i2c_write(0x2);									// write command byte
				i2c_write(u_motorByte0.motorByte0);                       // write first byte of output
       	 		i2c_write(u_motorByte1.motorByte1);                       // write second byte of output
       		 	i2c_stop();                            // set stop conditon = release bus

				i2c_start(I2C2+I2C_WRITE);
				i2c_write(0x2);
				i2c_write(u_outputByte0.outputByte0);
				i2c_write(u_outputByte1.outputByte1);

				counter ++;
				delay_ms(1);
   		 	}
		}


		if(OrangutanDigital::isInputHigh(IO_D1) && !OrangutanDigital::isInputHigh(IO_D2)){
			OrangutanLCD::gotoXY(7,0);
			OrangutanLCD::print("SECOND B");
			while(state != -1)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				if(state != 0){
					OrangutanLCD::gotoXY(0,1);
					OrangutanLCD::print("STATE ");
					OrangutanLCD::print(state);
				}

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

				//state conversions
	
				if(state == 0 && button == 0){
					button = 1;
					state = 1;
					counterRef = counter;
				}
				if(state == 1 && counter - counterRef > totallength1*m1LoadPlate/*&& u_inputByte0.bits_in_inputByte0.plate == 0*/){
						state = 2;
					counterRef = counter;
				}
				if(state == 2 && counter - counterRef > pWait){
					state = 3;
					counterRef = counter;
				}
				if(state == 3 && counter - counterRef > totallength2*m2HomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state = 4;
					counterRef = counter;
				}
				if(state == 4 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state = 5;
					counterRef = counter;
				}
				if(state == 5 && counter - counterRef > pWait){
						state = 6;
					counterRef = counter;
				}
				if(state == 6 && counter - counterRef > totallength2*m2Brush2StepWhole /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state = 15;
					counterRef = counter;
				}
				if(state == 15 && counter - counterRef > mWait){
					state = 16;
					counterRef = counter;
				}
				if(state == 16 && counter - counterRef > pWait){
					state = 17;
					counterRef = counter;
				}
				if(state == 17 && counter - counterRef > totallength2*m2HalfPlate){
					state = 18;
					counterRef = counter;
				}
				if(state == 18 && counter - counterRef > pWait){
					state = 19;
					counterRef = counter;
				}
				if(state == 19 && counter - counterRef > totallength2*m2HalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush2 == 0*/){
					state = 20;
					counterRef = counter;
				}
				if(state == 20 && counter - counterRef > pWait){
					state = 21;
					counterRef = counter;
				}
				if(state == 21 && counter - counterRef > totallength2*m2HalfPlate){
					state = 22;
					counterRef = counter;
				}
				if(state == 22 && counter - counterRef > pWait){
					state = 23;
					counterRef = counter;
				}
				if(state == 23 && counter - counterRef > totallength2*m2Dry1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry1 == 0*/){
					state = 24;
					counterRef = counter;
				}
				if(state == 24 && counter - counterRef > kWait){
					state = 25;
					counterRef = counter;
				}
				if(state == 25 && counter - counterRef > totallength2*m2WholePlate){
					state = 26;
					counterRef26 = counter;
				}
				if(state == 26 && counter - counterRef > totallength2*m2Dry2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry2 == 0*/){
					state = 27;
					counterRefFive = counter;
				}
				if(state == 27 && counter - counterRefFive > mWait){
					state = 28;
					counterRef = counter;
				}
				if(state == 28 && counter - counterRef > pWait){
					state = 29;
					counterRef = counter;
				}
				if(state == 29 && counter - counterRef > totallength2*m2WholePlate){
					state = 30;
					counterRef30 = counter;
				}
				if(state == 30 && counter - counterRef > totallength2*m2LoadBack /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state = 31;
					counterRef = counter;
				}
					if(state == 31 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state = 32;
					counterRef = counter;
				}
				if(state == 32 && counter - counterRef > totallength2*m2HomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureHome == 0*/){
					state = 33;
					counterRef = counter;
				}
				if(state == 33 && counter - counterRef > pWait){
					state = 34;
					counterRef = counter;
				}
				if(state == 34 && counter - counterRef > totallength1*m1LoadPlate){
					state = 35;
					counterRef = counter;
				}
				if(state == 35 && counter - counterRef > 10){
					state = -1;
				}


				//state actions
						
				if(state == 0){
					if(u_inputByte0.bits_in_inputByte0.fixtureHome == 1){
						u_motorByte0.bits_in_motorByte0.m2Dir = 0;  //********* 0 is used as fixture backward ?cc?, 1 as forward ?c? **********
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						m2 = 1;
					}
					else{
						u_motorByte0.bits_in_motorByte0.m2Drop = 0;
						m2 = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						if(print0 == 1){
							OrangutanLCD::gotoXY(0,1);
							OrangutanLCD::print("PUSH TO START");
							print0 = 0;
						}
					}
				}
				if(state == 1){
					//OrangutanLCD::clear();
					u_outputByte0.bits_in_outputByte0.ACPower = 0;
					if((counter % 200) < 100){
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
					}
					else{
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					}
					m1 = 1;
					u_motorByte0.bits_in_motorByte0.m1Dir = 1;  //****** dir1 ******
					u_motorByte0.bits_in_motorByte0.m1Drop = 1;
					u_outputByte0.bits_in_outputByte0.plateStop = 0;
				}
				if(state == 2){
					u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.plateStop = 1;
						m1 = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 1;
					u_motorByte0.bits_in_motorByte0.m1Drop = 0;
				}	
				if(state == 3){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ****** dir2 *******
					m2 = 1;
				}
				if(state == 4){
						m2 = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
				}
				if(state == 5){
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 1;
				}
				//*********************************************
				if(state == 6){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
				}
				
				//*************************** may need to reverse order depending on dry station 1 positioning ***************************
				if(state == 15){
					m2 = 0;
					u_motorByte1.bits_in_motorByte1.m4Drop = 1;
					u_motorByte1.bits_in_motorByte1.m4Dir = 0;  // ******* dir4 *******
					/*if(print15 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 2");
						print15 = 0;
					}*/
				}
				if(state == 16){
					m4 = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
				}
				if(state == 17){
					m2 = 1;
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;  // ******* dir2 *******
				}
				if(state == 18){
					m2 = 0;
					m4 = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
				}
				if(state == 19){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 20){
					m2 = 0;
					u_motorByte1.bits_in_motorByte1.m4Dir = 0;  // ******* dir4 *******
					m4 = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
				}
				if(state == 21){
					m2 = 1;
				}
				if(state == 22){
					m4 = 0;
					m2 = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
					u_motorByte1.bits_in_motorByte1.m4Drop = 0;
				}
				//*************************************************
				if(state == 23){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 24){
					m2 = 0;
					u_outputByte1.bits_in_outputByte1.airKnife = 0;
						/*if(print24 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("DRY AIR");
						print24 = 0;
					}*/
				}
				if(state == 25){
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 26){
					u_outputByte1.bits_in_outputByte1.airKnife = 1;
				}
				if(state == 27){
					m2 = 0;
					u_motorByte1.bits_in_motorByte1.m5Drop = 1;
					u_motorByte1.bits_in_motorByte1.m5Dir = 0;
					m5 = 1;
				}
				if(state == 28){
					/*if(print28 == 1){
							OrangutanLCD::clear();
						OrangutanLCD::print("FINAL DRY");
						print28 = 0;
						}*/
					u_outputByte1.bits_in_outputByte1.ptRaise = 0;
				}
				if(state == 29){
					m2 = 1;
				}
				if(state == 30){
					m5 = 0;
						u_motorByte1.bits_in_motorByte1.m5Drop = 0;
					u_outputByte1.bits_in_outputByte1.ptRaise = 1;
					u_outputByte1.bits_in_outputByte1.ptLower = 0;
				}
				if(state == 31){
					m2 = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
				}
				if(state == 32){
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;
					m2 = 1;
				}
				if(state == 33){
					m2 = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 0;
					u_motorByte0.bits_in_motorByte0.m1Drop = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
				}
				if(state == 34){
					u_motorByte0.bits_in_motorByte0.m1Dir = 0;
					m1 = 1;
					}
				if(state == 35){
					m1 = 0;
					u_motorByte0.bits_in_motorByte0.m1Drop = 0;
					if(print35 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("END OF CYCLE");
						print35 = 0;
					}
				}





				if( ((counter - counterRef) % (totallength1) ) < (steplength1) && m1)  //check if it is in the right period of the loop to send high
				{
					u_motorByte0.bits_in_motorByte0.m1Step = 1; // set bit 0
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m1Step = 0; // set bit 0
				}

				if( ((counter - counterRef) % (totallength2) ) < (steplength2) && m2)
				{
					u_motorByte0.bits_in_motorByte0.m2Step = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m2Step = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength3) ) < (steplength3) && m3)
				{
					u_motorByte0.bits_in_motorByte0.m3Step = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m3Step = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength4) ) < (steplength4) && m4)
				{
					u_motorByte1.bits_in_motorByte1.m4Step = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.m4Step = 0; // set bit 1
				}

				if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && m5)
				{
					u_motorByte1.bits_in_motorByte1.m5Step = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.m5Step = 0; // set bit 1
				}

				i2c_start(I2C1+I2C_WRITE);
				i2c_write(0x2);									// write command byte
				i2c_write(u_motorByte0.motorByte0);                       // write first byte of output
       	 		i2c_write(u_motorByte1.motorByte1);                       // write second byte of output
       		 	i2c_stop();                            // set stop conditon = release bus

				i2c_start(I2C2+I2C_WRITE);
				i2c_write(0x2);
				i2c_write(u_outputByte0.outputByte0);
				i2c_write(u_outputByte1.outputByte1);

				counter ++;
				delay_ms(1);
   		 	}
		}


		if(!OrangutanDigital::isInputHigh(IO_D1) && OrangutanDigital::isInputHigh(IO_D2)){
			OrangutanLCD::gotoXY(7,0);
			OrangutanLCD::print("BOTH B");
			while(state != -1)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				if(state != 0){
					OrangutanLCD::gotoXY(0,1);
					OrangutanLCD::print("STATE ");
					OrangutanLCD::print(state);
				}

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

				//state conversions
	
				if(state == 0 && button == 0){
					button = 1;
					state = 1;
					counterRef = counter;
				}
				if(state == 1 && counter - counterRef > totallength1*m1LoadPlate/*&& u_inputByte0.bits_in_inputByte0.plate == 0*/){
						state = 2;
					counterRef = counter;
				}
				if(state == 2 && counter - counterRef > pWait){
					state = 3;
					counterRef = counter;
				}
				if(state == 3 && counter - counterRef > totallength2*m2HomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state = 4;
					counterRef = counter;
				}
				if(state == 4 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state = 5;
					counterRef = counter;
				}
				if(state == 5 && counter - counterRef > pWait){
						state = 6;
					counterRef = counter;
				}
				if(state == 6 && counter - counterRef > totallength2*m2Brush1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state = 7;
					counterRef = counter;
				}
				if(state == 7 && counter - counterRef > mWait){
					state = 8;
					counterRef = counter;
				}
				if(state == 8 && counter - counterRef > pWait){
					state = 9;
						counterRef = counter;
				}
					if(state == 9 && counter - counterRef > totallength2*m2HalfPlate){
					state = 10;
					counterRef = counter;
				}
				if(state == 10 && counter - counterRef > pWait){
					state = 11;
				}
				if(state == 11 && counter - counterRef > totallength2*m2HalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state = 12;
					counterRef = counter;
				}
				if(state == 12 && counter - counterRef > pWait){
					state = 13;
					counterRef = counter;
				}
				if(state == 13 && counter - counterRef > totallength2*m2HalfPlate){
					state = 14;
					counterRef14 = counter;
					}
				if(state == 14 && counter - counterRef > totallength2*m2Brush2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush2*/){
					state = 15;
					counterRef = counter;
				}
				if(state == 15 && counter - counterRef > mWait){
					state = 16;
					counterRef = counter;
				}
				if(state == 16 && counter - counterRef > pWait){
					state = 17;
					counterRef = counter;
				}
				if(state == 17 && counter - counterRef > totallength2*m2HalfPlate){
					state = 18;
					counterRef = counter;
				}
				if(state == 18 && counter - counterRef > pWait){
					state = 19;
					counterRef = counter;
				}
				if(state == 19 && counter - counterRef > totallength2*m2HalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush2 == 0*/){
					state = 20;
					counterRef = counter;
				}
				if(state == 20 && counter - counterRef > pWait){
					state = 21;
					counterRef = counter;
				}
				if(state == 21 && counter - counterRef > totallength2*m2HalfPlate){
					state = 22;
					counterRef = counter;
				}
				if(state == 22 && counter - counterRef > pWait){
					state = 23;
					counterRef = counter;
				}
				if(state == 23 && counter - counterRef > totallength2*m2Dry1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry1 == 0*/){
					state = 24;
					counterRef = counter;
				}
				if(state == 24 && counter - counterRef > kWait){
					state = 25;
					counterRef = counter;
				}
				if(state == 25 && counter - counterRef > totallength2*m2WholePlate){
					state = 26;
					counterRef26 = counter;
				}
				if(state == 26 && counter - counterRef > totallength2*m2Dry2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry2 == 0*/){
					state = 27;
					counterRefFive = counter;
				}
				if(state == 27 && counter - counterRefFive > mWait){
					state = 28;
					counterRef = counter;
				}
				if(state == 28 && counter - counterRef > pWait){
					state = 29;
					counterRef = counter;
				}
				if(state == 29 && counter - counterRef > totallength2*m2WholePlate){
					state = 30;
					counterRef30 = counter;
				}
				if(state == 30 && counter - counterRef > totallength2*m2LoadBack /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state = 31;
					counterRef = counter;
				}
					if(state == 31 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state = 32;
					counterRef = counter;
				}
				if(state == 32 && counter - counterRef > totallength2*m2HomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureHome == 0*/){
					state = 33;
					counterRef = counter;
				}
				if(state == 33 && counter - counterRef > pWait){
					state = 34;
					counterRef = counter;
				}
				if(state == 34 && counter - counterRef > totallength1*m1LoadPlate){
					state = 35;
					counterRef = counter;
				}
				if(state == 35 && counter - counterRef > 10){
					state = -1;
				}


				//state actions
						
				if(state == 0){
					if(u_inputByte0.bits_in_inputByte0.fixtureHome == 1){
						u_motorByte0.bits_in_motorByte0.m2Dir = 0;  //********* 0 is used as fixture backward ?cc?, 1 as forward ?c? **********
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						m2 = 1;
					}
					else{
						u_motorByte0.bits_in_motorByte0.m2Drop = 0;
						m2 = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						if(print0 == 1){
							OrangutanLCD::gotoXY(0,1);
							OrangutanLCD::print("PUSH TO START");
							print0 = 0;
						}
					}
				}
				if(state == 1){
					//OrangutanLCD::clear();
					u_outputByte0.bits_in_outputByte0.ACPower = 0;
					if((counter % 200) < 100){
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
					}
					else{
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					}
					m1 = 1;
					u_motorByte0.bits_in_motorByte0.m1Dir = 1;  //****** dir1 ******
					u_motorByte0.bits_in_motorByte0.m1Drop = 1;
					u_outputByte0.bits_in_outputByte0.plateStop = 0;
				}
				if(state == 2){
					u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.plateStop = 1;
						m1 = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 1;
					u_motorByte0.bits_in_motorByte0.m1Drop = 0;
				}	
				if(state == 3){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ****** dir2 *******
					m2 = 1;
				}
				if(state == 4){
						m2 = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
				}
				if(state == 5){
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 1;
				}
				//*********************************************
				if(state == 6){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 7){
					m2 = 0;
					u_motorByte1.bits_in_motorByte1.m3Drop = 1;
					u_motorByte0.bits_in_motorByte0.m3Dir = 0;  // ******* dir3 *******
					/*if(print6 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 1");
						print6 = 0;
					}*/
				}
				if(state == 8){
					m3 = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
				}
				if(state == 9){
					m2 = 1;
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;  // ******* dir2 *******
				}
				if(state == 10){
					m2 = 0;
					m3 = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
				}
				if(state == 11){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 12){
					m2 = 0;
					u_motorByte0.bits_in_motorByte0.m3Dir = 0;  // ******* dir3 *******
					m3 = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
				}
				if(state == 13){
					m2 = 1;
				}
				if(state == 14){
					m3 = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
					u_motorByte1.bits_in_motorByte1.m3Drop = 0;
				}
				//*************************** may need to reverse order depending on dry station 1 positioning ***************************
				if(state == 15){
					m2 = 0;
					u_motorByte1.bits_in_motorByte1.m4Drop = 1;
					u_motorByte1.bits_in_motorByte1.m4Dir = 0;  // ******* dir4 *******
					/*if(print15 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 2");
						print15 = 0;
					}*/
				}
				if(state == 16){
					m4 = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
				}
				if(state == 17){
					m2 = 1;
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;  // ******* dir2 *******
				}
				if(state == 18){
					m2 = 0;
					m4 = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
				}
				if(state == 19){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 20){
					m2 = 0;
					u_motorByte1.bits_in_motorByte1.m4Dir = 0;  // ******* dir4 *******
					m4 = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
				}
				if(state == 21){
					m2 = 1;
				}
				if(state == 22){
					m4 = 0;
					m2 = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
					u_motorByte1.bits_in_motorByte1.m4Drop = 0;
				}
				//*************************************************
				if(state == 23){
					u_motorByte0.bits_in_motorByte0.m2Dir = 1;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 24){
					m2 = 0;
					u_outputByte1.bits_in_outputByte1.airKnife = 0;
						/*if(print24 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("DRY AIR");
						print24 = 0;
					}*/
				}
				if(state == 25){
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;  // ******* dir2 *******
					m2 = 1;
				}
				if(state == 26){
					u_outputByte1.bits_in_outputByte1.airKnife = 1;
				}
				if(state == 27){
					m2 = 0;
					u_motorByte1.bits_in_motorByte1.m5Drop = 1;
					u_motorByte1.bits_in_motorByte1.m5Dir = 0;
					m5 = 1;
				}
				if(state == 28){
					/*if(print28 == 1){
							OrangutanLCD::clear();
						OrangutanLCD::print("FINAL DRY");
						print28 = 0;
						}*/
					u_outputByte1.bits_in_outputByte1.ptRaise = 0;
				}
				if(state == 29){
					m2 = 1;
				}
				if(state == 30){
					m5 = 0;
						u_motorByte1.bits_in_motorByte1.m5Drop = 0;
					u_outputByte1.bits_in_outputByte1.ptRaise = 1;
					u_outputByte1.bits_in_outputByte1.ptLower = 0;
				}
				if(state == 31){
					m2 = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
				}
				if(state == 32){
					u_motorByte0.bits_in_motorByte0.m2Dir = 0;
					m2 = 1;
				}
				if(state == 33){
					m2 = 0;
					u_motorByte0.bits_in_motorByte0.m2Drop = 0;
					u_motorByte0.bits_in_motorByte0.m1Drop = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
				}
				if(state == 34){
					u_motorByte0.bits_in_motorByte0.m1Dir = 0;
					m1 = 1;
					}
				if(state == 35){
					m1 = 0;
					u_motorByte0.bits_in_motorByte0.m1Drop = 0;
					if(print35 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("END OF CYCLE");
						print35 = 0;
					}
				}





				if( ((counter - counterRef) % (totallength1) ) < (steplength1) && m1)  //check if it is in the right period of the loop to send high
				{
					u_motorByte0.bits_in_motorByte0.m1Step = 1; // set bit 0
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m1Step = 0; // set bit 0
				}

				if( ((counter - counterRef) % (totallength2) ) < (steplength2) && m2)
				{
					u_motorByte0.bits_in_motorByte0.m2Step = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m2Step = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength3) ) < (steplength3) && m3)
				{
					u_motorByte0.bits_in_motorByte0.m3Step = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.m3Step = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength4) ) < (steplength4) && m4)
				{
					u_motorByte1.bits_in_motorByte1.m4Step = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.m4Step = 0; // set bit 1
				}

				if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && m5)
				{
					u_motorByte1.bits_in_motorByte1.m5Step = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.m5Step = 0; // set bit 1
				}

				i2c_start(I2C1+I2C_WRITE);
				i2c_write(0x2);									// write command byte
				i2c_write(u_motorByte0.motorByte0);                       // write first byte of output
       	 		i2c_write(u_motorByte1.motorByte1);                       // write second byte of output
       		 	i2c_stop();                            // set stop conditon = release bus

				i2c_start(I2C2+I2C_WRITE);
				i2c_write(0x2);
				i2c_write(u_outputByte0.outputByte0);
				i2c_write(u_outputByte1.outputByte1);

				counter ++;
				delay_ms(1);
   		 	}
		}
		}
		u_motorByte0.bits_in_motorByte0.m1Drop = 0;
		u_motorByte0.bits_in_motorByte0.m2Drop = 0;
		u_motorByte1.bits_in_motorByte1.m3Drop = 0;
		u_motorByte1.bits_in_motorByte1.m4Drop = 0;
		u_motorByte1.bits_in_motorByte1.m5Drop = 0;
		
		i2c_start(I2C1+I2C_WRITE);
		i2c_write(0x2);
		i2c_write(u_motorByte0.motorByte0);
		i2c_write(u_motorByte1.motorByte1);
		i2c_stop();
	}
	OrangutanLCD::print("END");
	}
}


void motor_and_write(int counter, int counterRef, int counterRefFive, int m1, int m2, int m3, int m4, int m5)
{
//motor and write
	if( ((counter - counterRef) % (totallength1) ) < (steplength1) && m1)  //check if it is in the right period of the loop to send high
	{
		u_motorByte0.bits_in_motorByte0.m1Step = 1; // set bit 0
	}
	else
	{
		u_motorByte0.bits_in_motorByte0.m1Step = 0; // set bit 0
	}
		if( ((counter - counterRef) % (totallength2) ) < (steplength2) && m2)
	{
		u_motorByte0.bits_in_motorByte0.m2Step = 1; // set bit 1
	}
	else
	{
		u_motorByte0.bits_in_motorByte0.m2Step = 0; // set bit 1
	}
	if( ((counter - counterRef) % (totallength3) ) < (steplength3) && m3)
	{
		u_motorByte0.bits_in_motorByte0.m3Step = 1; // set bit 1
	}
	else
	{
		u_motorByte0.bits_in_motorByte0.m3Step = 0; // set bit 1
	}

	if( ((counter - counterRef) % (totallength4) ) < (steplength4) && m4)
	{
		u_motorByte1.bits_in_motorByte1.m4Step = 1; // set bit 1
	}
	else
	{
		u_motorByte1.bits_in_motorByte1.m4Step = 0; // set bit 1
	}

	if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && m5)
	{
		u_motorByte1.bits_in_motorByte1.m5Step = 1; // set bit 1
	}
	else
	{
		u_motorByte1.bits_in_motorByte1.m5Step = 0; // set bit 1
	}

	i2c_start(I2C1+I2C_WRITE);
	i2c_write(0x2);									// write command byte
	i2c_write(u_motorByte0.motorByte0);                       // write first byte of output
	i2c_write(u_motorByte1.motorByte1);                       // write second byte of output
	i2c_stop();                            // set stop conditon = release bus

	i2c_start(I2C2+I2C_WRITE);
	i2c_write(0x2);
	i2c_write(u_outputByte0.outputByte0);
	i2c_write(u_outputByte1.outputByte1);
}
