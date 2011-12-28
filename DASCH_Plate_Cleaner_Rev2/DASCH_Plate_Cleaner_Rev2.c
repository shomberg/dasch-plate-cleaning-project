/*
DASCH Plate Cleaning Project _ Bob Simcoe, Steven Homberg
http://hea_www.harvard.edu/DASCH/
Runs on Orangutan SVP 1284p board

The program runs the plate cleaning machine in one of two modes: Maintenance mode, in which
the device's hardware can be checked for proper functionality, and Run mode, in whcih a
plate can be cleaned in several ways.

Maintenance mode is for troubleshooting.  In Maintenance mode there are 4 sub-modes.  The
first tests all of the inputs, the second tests all of the outputs, the third tests all
of the motors, and the fourth and final sub-mode tests all of the inputs, outputs, and
motors before stopping.  The four-way switch can be changed durin operation to change sub-mode.

Run mode is for cleaning plates.  In Run mode, there are 3 sub-modes.  The first cleans a
plate using only the first brush, the second cleans a plate using only the second brush,
and the third cleans a plate using both brushes.  A fourth to be added soon will clean with both
while pausing between states to allow for troubleshooting.  Sub-modes cannot be changed in
mid-run.

*/

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


  
void motor_and_write(int counter, int counterRef, int counterRefFive, int plateLoadMotor, int fixtureMotor, int brush1Motor, int brush2Motor, int paperTowelMotor, int totallength1, int totallength2, int totallength3, int totallength4, int totallength5, int steplength1, int steplength2, int steplength3, int steplength4, int steplength5);

//int button_debounce(int counter, int *pcounterRefPush, int *pcounterRefRel, int *pstateButton);

bool button_debounce(int counter, int *pstateButton);


int main()   
{
    
    unsigned char ret1, ret2, ret3;												// Used to determine if the I2Cs return properly
	int fixtureMotorHalfPlate = 2000;	//Number of steps of the fixture motor to move 1/2 plate length
	int fixtureMotorWholePlate = 2000;	//Number of steps of the fixture motor to move one plate length
	int plateLoadMotorLoadPlate = 2000;	//Number of steps of the plate load motor to load the plate
	int fixtureMotorHomeFix = 2000;		//Number of steps of the fixture motor to home the fixture
	int fixtureMotorBrush1Step = 2000;	//Number of steps of the fixture motor to move to the first brush
	int fixtureMotorBrush2Step = 2000;	//Number of steps of the fixture motor to move to the second brush
	int fixtureMotorDry1Step = 2000;	//Number of steps of the fixture motor to move to the first dry station
	int fixtureMotorDry2Step = 2000;	//Number of steps of the fixture motor to move to the second dry station
	int fixtureMotorLoadBack = 2000;	//Number of steps of the fixture motor to move back to the loading station
	int fixtureMotorDry1StepWhole = 2000;	//Number of steps of the fixture motor to move to the first drying station skipping a brush
	int fixtureMotorBrush2StepWhole = 2000;	//Number of steps of the fixture motor to move to the second brush skipping a brush

					//mx holds whether motor x should move: 0 = no, 1 = yes
	int plateLoadMotor = 0;		//plate load motor
	int fixtureMotor = 0;		//fixture motor
	int brush1Motor = 0;		//brush 1 motor
	int brush2Motor = 0;		//brush 2 motor
	int paperTowelMotor = 0; 	//paper towel roller motor
	int state = 0;										//Holds program's current state
	int counter = 0;									//Counts iteration of the loop for timing purposes
	int last = 0;										//In Debug Cycle, holds program's most recent non-waiting state

	//various reference points used by the program to tell how long since an event has occurred
	//the ones with numbers correspond to run mode states, and Five refers to motor 5 (paper towel roller)
	int counterRef = 0;
	int counterRef14 = 0;
	int counterRef26 = 0;
	int counterRefFive = 0;
	int counterRef30 = 0;
		
	//holds wait times for various actions in ms
	int pWait = 100;
	int  mWait = 100;
	int  kWait = 100;

	//hold the length of the high and high-low periods for the various motors - this controls their speed
	int steplength1 = 1;
	int  steplength2 = 1;
	int  steplength3 = 1;
	int  steplength4 = 1;
	int  steplength5 = 1;
	int totallength1 = 2;
	int  totallength2 = 2;
	int totallength3 = 2;
	int  totallength4 = 2;
	int  totallength5 = 2;
		
	//hold whether or not statements have been printed yet (numbers refer to run mode states)
	int print0 = 1;
	int  print35 = 1;
		
	//initialize button variables
	int button = 1;				
	int stateButton = NONE;			//holds the state in the button subroutine where the program is currently

	bool buttonTriggered = false;	//buttonTriggered is false if it hasn't been triggered, true if it has;
    
	while(1){						//Repeats the entire program indefinitely (runs maintenance or normal each time)

							//mx holds whether motor x should move: 0 = no, 1 = yes
	plateLoadMotor = 0;		//plate load motor
	fixtureMotor = 0;		//fixture motor
	brush1Motor = 0;		//brush 1 motor
	brush2Motor = 0;		//brush 2 motor
	paperTowelMotor = 0; 	//paper towel roller motor
	state = 0;										//Holds program's current state
	counter = 0;									//Counts iteration of the loop for timing purposes
	last = 0;										//In Debug Cycle, holds program's most recent non-waiting state

	//various reference points used by the program to tell how long since an event has occurred
	//the ones with numbers correspond to run mode states, and Five refers to motor 5 (paper towel roller)
	counterRef = 0;
	counterRef14 = 0;
	counterRef26 = 0;
	counterRefFive = 0;
	counterRef30 = 0;
		
	//holds wait times for various actions in ms
	pWait = 100;
	mWait = 100;
	kWait = 100;

	//hold the length of the high and high-low periods for the various motors - this controls their speed
	steplength1 = 1;
	steplength2 = 1;
	steplength3 = 1;
	steplength4 = 1;
	steplength5 = 1;
	totallength1 = 2;
	totallength2 = 2;
	totallength3 = 2;
	totallength4 = 2;
	totallength5 = 2;
	
	//hold whether or not statements have been printed yet (numbers refer to run mode states)
	print0 = 1;
	print35 = 1;
	
	//initialize button variables
	button = 1;				//button = 0 means it has been debounced, button = 1 means it hasn't
	buttonTriggered = false;
	stateButton = NONE;		//holds the state in the button subroutine where the program is currently
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
	
    ret1 = i2c_start(I2C1+I2C_WRITE);       // ret1 holds whether or not I2C1 started properly
	i2c_stop();
	ret2 = i2c_start(I2C2+I2C_WRITE);       // ret2 holds whether or not I2C2 started properly
	i2c_stop();
	ret3 = i2c_start(I2C3+I2C_WRITE);       // ret3 holds whether or not I2C3 started properly
	i2c_stop();
    
	OrangutanLCD::clear();
	OrangutanLCD::print("START ");
	
	if ( ret1 | ret2 | ret3 ) {			// Check if all three I2C devices responded properly
        /* failed to issue start condition(s), possibly no device found */
        
		OrangutanLCD::print("NAK ");
		i2c_stop();
		OrangutanLCD::print("STOP ");
    }
	else {
		OrangutanLCD::print("ACK ");
        /* issuing start conditions ok, devices accessible */
		OrangutanLCD::print("BINIT ");
		i2c_start(I2C1+I2C_WRITE);		//Configures I2C1 registers as outputs
		i2c_write(0x6);
		i2c_write(0x0);
		i2c_write(0x0);
		i2c_stop();
		
		i2c_start(I2C2+I2C_WRITE);		//Configures I2C2 registers as outputs
		i2c_write(0x6);
		i2c_write(0x0);
		i2c_write(0x0);
		i2c_stop();
										//Registers default to input, so I2C3 is fine already


		delay_ms(1000);
		OrangutanLCD::clear();


		if(OrangutanDigital::isInputHigh(IO_D3))							//Tests if the switch is set to Maintenance Mode
		{
			OrangutanLCD::print("MAINTENANCE");
			delay_ms(1000);
			
			//initialize
			int print = 0;
			int submode = -1;

			OrangutanLCD::clear();
			OrangutanLCD::print("SELECT MODE");
			
			//Repeats until user presses and releases button - waiting for user to select a mode
			while(!buttonTriggered){
				/*if(stateButton == NONE && OrangutanDigital::isInputHigh(IO_D0)){
					counterRefPush = counter;
					stateButton = PRESSED;
				}
				if(stateButton == PRESSED){
					if(counter - counterRefPush > 15){
						if(!OrangutanDigital::isInputHigh(IO_D0)){
							counterRefRel = counter;
							stateButton = RELEASED;
						}
					}
					else if(!OrangutanDigital::isInputHigh(IO_D0)){
						stateButton = NONE;
					}
				}
				if(stateButton == RELEASED){
					if(OrangutanDigital::isInputHigh(IO_D0)){
						stateButton = PRESSED;
						counterRefPush = counter;
					}
					else if(counter - counterRefRel > 15){
						button = 0;
						stateButton = NONE;
					}
				}*/
				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);
				counter++;
				delay_ms(1);
			}
			buttonTriggered = false;
			counter = 0;
			OrangutanLCD::clear();
			

			//Repeats entire maintenance mode cycle until it's finished
			while(state != DONEM){

				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
				/*if(stateButton == 0 && OrangutanDigital::isInputHigh(IO_D0)){
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
				}*/

				//User presses button for each state transition
				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);
				

				/*Switches mode to whichever is currently selected with submode selection switch
				Modes are:	Input only 		--     starts testing each input individually and ends after testing all inputs
							Output only 	--     starts testing each output individually and ends after testing all outputs
							Motor only		--     starts testing each motor individually and ends after testing all motors
							All				--     starts testing each input, then output, then motor, and ends after testing each

				*/
				if(!OrangutanDigital::isInputHigh(IO_D1) && !OrangutanDigital::isInputHigh(IO_D2) && submode != 0){
					state = S0;
					submode = INPUT;
					OrangutanLCD::clear();
					OrangutanLCD::print("INPUTS ");
					OrangutanLCD::gotoXY(0,1);
					print = 0;
				}
				if(OrangutanDigital::isInputHigh(IO_D1) && !OrangutanDigital::isInputHigh(IO_D2) && submode != 1){
					state = O0_ON;
					submode = OUTPUT;
					OrangutanLCD::clear();
					OrangutanLCD::print("OUTPUTS");
					OrangutanLCD::gotoXY(0,1);
					print = 0;
				}
				if(!OrangutanDigital::isInputHigh(IO_D1) && OrangutanDigital::isInputHigh(IO_D2) && submode != 2){
					state = M1_F;
					submode = MOTOR;
					OrangutanLCD::clear();
					OrangutanLCD::print("MOTORS ");
					OrangutanLCD::gotoXY(0,1);
					print = 0;
				}
				if(OrangutanDigital::isInputHigh(IO_D1) && OrangutanDigital::isInputHigh(IO_D2) && submode != 3){
					state = S0;
					submode = ALL;
					OrangutanLCD::clear();
					OrangutanLCD::print("INPUTS ");
					OrangutanLCD::gotoXY(0,1);
					print = 0;
				}

				//state transitions

				//Has button been pressed?
				if(buttonTriggered) {					//if a state conversion is in order (button press)
					if(state <= S7){				//if it's doing the sensors
						OrangutanLCD::gotoXY(0,1);
						buttonTriggered = false;
						state ++;
						counterRef = counter;
						print = 0;
						if(state == S7 + 1){		//accounts for possible end of maintenance mode after sensors
							OrangutanLCD::clear();
							if(submode == 0){
								state = DONEM;
							}
							else{
								OrangutanLCD::print("OUTPUTS");
								OrangutanLCD::gotoXY(0,1);
							}
						}
					}
					else if(state <= O11_OFF){		//if it's doing the outputs
						if(state % 2 == 0){			//current state is an 'on' state
							OrangutanLCD::gotoXY(13,1);
							OrangutanLCD::print("OFF");
						}
						else{						//current state is an 'off' state
							OrangutanLCD::gotoXY(0,1);
						}
						buttonTriggered = false;
						state ++;
						counterRef = counter;
						print = 0;
						if(state == O11_OFF + 1){	//accounts for possible end of maintenance mode after outputs
							OrangutanLCD::clear();
							if(submode == 1){
								state = DONEM;
							}
							else{
								u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
								OrangutanLCD::print("MOTORS");
								OrangutanLCD::gotoXY(0,1);
							}
						}
					}
					else{							//if it's doing the motors
						buttonTriggered = false;
						state ++;
						counterRef = counter;
						print = 0;
						if((state - 1) % 2 == 0){	//if the current state is a 'front' state
							OrangutanLCD::gotoXY(11,1);
							OrangutanLCD::print("BACK ");
						}
						else{						//if the current state is a 'back' state
							switch (state) {
								case M1_B + 1:
									OrangutanLCD::gotoXY(0,1);
									u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
									u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
									break;
								case M2_B + 1:
									OrangutanLCD::gotoXY(0,1);
									u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
									u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 1;
									break;
								case M3_B + 1:
									OrangutanLCD::gotoXY(0,1);
									u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 0;
									u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 1;
									break;
								case M4_B + 1:
									OrangutanLCD::gotoXY(0,1);
									u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 0;
									u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 1;
									break;
								case M5_B + 1:
									OrangutanLCD::clear();
									u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 0;
									state = DONEM;
									paperTowelMotor = 0;
									break;
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
					if(submode == 0){
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
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
					button = 1;
					if(submode == 1){
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
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
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
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
					u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 1;
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
					u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 0;
					u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 1;
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
					u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 0;
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 1;
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
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 0;
					button = 1;
					state = -1;
					print = 0;
					paperTowelMotor = 0;
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
						break;
					case S1:
						if(print == 0){
							OrangutanLCD::print("SENSOR 1");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureLift);
						break;
					case S2:
						if(print == 0){
							OrangutanLCD::print("SENSOR 2");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureHome);
						break;
					case S3:
						if(print == 0){
							OrangutanLCD::print("SENSOR 3");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixturePlate);
						break;
					case S4:
						if(print == 0){
							OrangutanLCD::print("SENSOR 4");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureBrush1);
						break;
					case S5:
						if(print == 0){
							OrangutanLCD::print("SENSOR 5");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureBrush2);
						break;
					case S6:
						if(print == 0){
							OrangutanLCD::print("SENSOR 6");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureDry1);
						break;
					case S7:
						if(print == 0){
							OrangutanLCD::print("SENSOR 7");
							print = 1;
						}
						OrangutanLCD::gotoXY(15,1);
						OrangutanLCD::print(u_inputByte0.bits_in_inputByte0.fixtureDry2);
						break;
					case O0_ON:
						if(print == 0){
							OrangutanLCD::print("AC POWER     ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.ACPower = 0;
						break;
					case O0_OFF:
						u_outputByte0.bits_in_outputByte0.ACPower = 1;
						break;
					case O1_ON:
						if(print == 0){
							OrangutanLCD::print("BLOWER       ON  ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
						break;
					case O1_OFF:
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
						break;
					case O2_ON:
						if(print == 0){
							OrangutanLCD::print("PLATE STOP   ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.plateStop = 0;
						break;
					case O2_OFF:
						u_outputByte0.bits_in_outputByte0.plateStop = 1;
						break;
					case O3_ON:
						if(print == 0){
							OrangutanLCD::print("PLATE RAISE  ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
						break;
					case O3_OFF:
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						//u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						break;
					case O4_ON:
						if(print == 0){
							OrangutanLCD::print("PLATE LOWER  ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						break;
					case O4_OFF:
						u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
						break;
					case O5_ON:
						if(print == 0){
							OrangutanLCD::print("BRUSH1 RAISE ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
						u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
						break;
					case O5_OFF:
						u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
						//u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
						break;
					case O6_ON:
						if(print == 0){
							OrangutanLCD::print("BRUSH1 LOWER ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
						u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
						break;
					case O6_OFF:
						u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
						break;
					case O7_ON:
						if(print == 0){
							OrangutanLCD::print("BRUSH2 RAISE ON ");
							print = 1;
						}
						u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
						u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
						break;
					case O7_OFF:
						u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
						//u_outputByte0.bits_in_outputByte0.brush2Lower = 0;
						break;
					case O8_ON:
						if(print == 0){
							OrangutanLCD::print("BRUSH2 LOWER ON ");
							print = 1;
						}
						u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
						u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
						break;
					case O8_OFF:
						u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
						break;
					case O9_ON:
						if(print == 0){
							OrangutanLCD::print("PAPER RAISE  ON ");
							print = 1;
						}
						u_outputByte1.bits_in_outputByte1.ptRaise = 0;
						u_outputByte1.bits_in_outputByte1.ptLower = 1;
						break;
					case O9_OFF:
						u_outputByte1.bits_in_outputByte1.ptRaise = 1;
						//u_outputByte1.bits_in_outputByte1.ptLower = 0;
						break;
					case O10_ON:
						if(print == 0){
							OrangutanLCD::print("PAPER LOWER  ON ");
							print = 1;
						}
						u_outputByte1.bits_in_outputByte1.ptLower = 0;
						u_outputByte1.bits_in_outputByte1.ptRaise = 1;
						break;
					case O10_OFF:
						u_outputByte1.bits_in_outputByte1.ptLower = 1;
						break;
					case O11_ON:
						if(print == 0){
							OrangutanLCD::print("AIR KNIFE    ON ");
							print = 1;
						}
						u_outputByte1.bits_in_outputByte1.airKnife = 0;
						break;
					case O11_OFF:
						u_outputByte1.bits_in_outputByte1.airKnife = 1;
						break;
					case M1_F:
						if(print == 0){
							OrangutanLCD::print("LOADING    FRONT");
							print = 1;
						}
						u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 0;
						plateLoadMotor =  1;
						break;
					case M1_B:
						u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 1;
						plateLoadMotor =  1;
						break;
					case M2_F:
						if(print == 0){
							OrangutanLCD::print("DRIVE      FRONT");
							print = 1;
						}
						u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;
						fixtureMotor =  1;
						plateLoadMotor = 0;
						break;
					case M2_B:
						u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;
						fixtureMotor =  1;
						break;
					case M3_F:
						if(print == 0){
							OrangutanLCD::print("BRUSH1     FRONT");
							print = 1;
						}
						u_motorByte0.bits_in_motorByte0.brush1MotorDir = 0;
						brush1Motor =  1;
						fixtureMotor = 0;
						break;
					case M3_B:
						u_motorByte0.bits_in_motorByte0.brush1MotorDir = 1;
						brush1Motor =  1;
						break;
					case M4_F:
						if(print == 0){
							OrangutanLCD::print("BRUSH2     FRONT");
							print = 1;
						}
						u_motorByte1.bits_in_motorByte1.brush2MotorDir = 0;
						brush2Motor =  1;
						brush1Motor = 0;
						break;
					case M4_B:
						u_motorByte1.bits_in_motorByte1.brush2MotorDir = 1;
						brush2Motor =  1;
						break;
					case M5_F:
						if(print == 0){
							OrangutanLCD::print("PAPER      FRONT");
							print = 1;
						}
						u_motorByte1.bits_in_motorByte1.paperTowelMotorDir = 0;
						paperTowelMotor =  1;
						brush2Motor = 0;
						break;
					case M5_B:
						u_motorByte1.bits_in_motorByte1.paperTowelMotorDir = 1;
						paperTowelMotor =  1;
						break;
				}

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, counterRefFive, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor, totallength1, totallength2, totallength3, totallength4, totallength5, steplength1, steplength2, steplength3, steplength4, steplength5);



				/*if( ((counter - counterRef) % (totallength1) ) < (steplength1) && plateLoadMotor)  //check if it is in the right period of the loop to send high
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 1; // set bit 0
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 0; // set bit 0
				}

				if( ((counter - counterRef) % (totallength2) ) < (steplength2) && fixtureMotor)
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength3) ) < (steplength3) && brush1Motor)
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength4) ) < (steplength4) && brush2Motor)
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && paperTowelMotor)
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 0; // set bit 1
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
		
		OrangutanLCD::clear();
		OrangutanLCD::print("SELECT CYCLE");
		//Repeats until user presses and releases button - waiting for user to select a mode
		while(!buttonTriggered){
			/*if(stateButton == NONE && OrangutanDigital::isInputHigh(IO_D0)){
				counterRefPush = counter;
				stateButton = PRESSED;
			}
			if(stateButton == PRESSED){
				if(counter - counterRefPush > 15){
					if(!OrangutanDigital::isInputHigh(IO_D0)){
						counterRefRel = counter;
						stateButton = RELEASED;
					}
				}
				else if(!OrangutanDigital::isInputHigh(IO_D0)){
					stateButton = NONE;
				}
			}
			if(stateButton == RELEASED){
				if(OrangutanDigital::isInputHigh(IO_D0)){
					stateButton = PRESSED;
					counterRefPush = counter;
				}
				else if(counter - counterRefRel > 15){
					button = 0;
					stateButton = NONE;
				}
			}*/
			buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);
			counter++;
			delay_ms(1);
		}
		buttonTriggered = false;
		counter = 0;
		OrangutanLCD::clear();
		OrangutanLCD::print("CYCLE:");

		//u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
		u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
		/*u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 1;
		u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 1;
		u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 1;*/

		if(!OrangutanDigital::isInputHigh(IO_D1) && !OrangutanDigital::isInputHigh(IO_D2)){
			OrangutanLCD::gotoXY(7,0);
			OrangutanLCD::print("FIRST B");
			while(state != DONER)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
				if(state != INIT){
					OrangutanLCD::gotoXY(0,1);
					OrangutanLCD::print("STATE ");
					OrangutanLCD::print(state);
				}

				/*if(stateButton == 0 && OrangutanDigital::isInputHigh(IO_D0)){
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
				}*/
				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);

				//state transitions
	
				if(state == INIT && buttonTriggered){
					buttonTriggered = false;
					state ++;
					counterRef = counter;
				}
				if(state == LOAD && /*counter - counterRef > totallength1*plateLoadMotorLoadPlate*/&& u_inputByte0.bits_in_inputByte0.plate == 0){
					state ++;
					counterRef = counter;
				}
				if(state == RAISEL1 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == FIXL && counter - counterRef > totallength2*fixtureMotorHomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == LOWERL1 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == LOWERL2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == MOVEC1 && counter - counterRef > totallength2*fixtureMotorBrush1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == B1SET && counter - counterRef > mWait){
					state ++;
					counterRef = counter;
				}
				if(state == B1START1 && counter - counterRef > pWait){
					state ++;
						counterRef = counter;
				}
				if(state == CLEAN1_1 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state ++;
					counterRef = counter;
				}
				if(state == B1STOP1 && counter - counterRef > pWait){
					state ++;
				}
				if(state == CLEAN1_2 && counter - counterRef > totallength2*fixtureMotorHalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == B1START2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == CLEAN1_3 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state ++;
					counterRef14 = counter;
					}
				if(state == B1STOP2 && counter - counterRef > pWait){
					state = 23;
					counterRef = counter;
				}
				if(state == MOVED1 && counter - counterRef > totallength2*fixtureMotorDry1StepWhole /*&& u_inputByte0.bits_in_inputByte0.fixtureDry1 == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == D1START && counter - counterRef > kWait){
					state ++;
					counterRef = counter;
				}
				if(state == DRY1 && counter - counterRef > totallength2*fixtureMotorWholePlate){
					state ++;
					counterRef26 = counter;
				}
				if(state == D1STOP && counter - counterRef > totallength2*fixtureMotorDry2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry2 == 0*/){
					state ++;
					counterRefFive = counter;
				}
				if(state == D2START && counter - counterRefFive > mWait){
					state ++;
					counterRef = counter;
				}
				if(state == D2RAISE && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == DRY2 && counter - counterRef > totallength2*fixtureMotorWholePlate){
					state ++;
					counterRef30 = counter;
				}
				if(state == D2STOP && counter - counterRef > totallength2*fixtureMotorLoadBack /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == RAISEL2 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == FIXH && counter - counterRef > totallength2*fixtureMotorHomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureHome == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == LOWERL3 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == UNLOAD && counter - counterRef > totallength1*plateLoadMotorLoadPlate){
					state ++;
					counterRef = counter;
				}
				if(state == END && counter - counterRef > 10){
					state = DONER;
				}


				//state actions
						
			switch (state){
				case INIT:
					if(u_inputByte0.bits_in_inputByte0.fixtureHome == 1){
						u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  //********* 0 is used as fixture backward ?cc?, 1 as forward ?c? **********
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						fixtureMotor = 1;
					}
					else{
						u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
						fixtureMotor = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						if(print0 == 1){
							OrangutanLCD::gotoXY(0,1);
							OrangutanLCD::print("PUSH TO START");
							print0 = 0;
						}
					}
					break;
				case LOAD:
					//OrangutanLCD::clear();
					u_outputByte0.bits_in_outputByte0.ACPower = 0;
					if((counter % 200) < 100){
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
					}
					else{
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					}
					plateLoadMotor = 1;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 1;  //****** dir1 ******
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
					u_outputByte0.bits_in_outputByte0.plateStop = 0;
					break;
				case RAISEL1:
					u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.plateStop = 1;
						plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
					break;
				case FIXL:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ****** dir2 *******
					fixtureMotor = 1;
					break;
				case LOWERL1:
						fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					break;
				case LOWERL2:
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
				//*********************************************
					break;
				case MOVEC1:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case B1SET:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 1;
					u_motorByte0.bits_in_motorByte0.brush1MotorDir = 0;  // ******* dir3 *******
					/*if(print6 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 1");
						print6 = 0;
					}*/
					break;
				case B1START1:
					brush1Motor = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
					break;
				case CLEAN1_1:
					fixtureMotor = 1;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					break;
				case B1STOP1:
					fixtureMotor = 0;
					brush1Motor = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
					break;
				case CLEAN1_2:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case B1START2:
					fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.brush1MotorDir = 0;  // ******* dir3 *******
					brush1Motor = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
					break;
				case CLEAN1_3:
					fixtureMotor = 1;
					break;
				case B1STOP2:
					brush1Motor = 0;
					fixtureMotor = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
				
					break;
				//*************************************************
				case MOVED1:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 0;
					break;
				case D1START:
					fixtureMotor = 0;
					u_outputByte1.bits_in_outputByte1.airKnife = 0;
						/*if(print24 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("DRY AIR");
						print24 = 0;
					}*/
					break;
				case DRY1:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case D1STOP:
					u_outputByte1.bits_in_outputByte1.airKnife = 1;
					break;
				case D2START:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 1;
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDir = 0;
					paperTowelMotor = 1;
					break;
				case D2RAISE:
					/*if(print28 == 1){
							OrangutanLCD::clear();
						OrangutanLCD::print("FINAL DRY");
						print28 = 0;
						}*/
					u_outputByte1.bits_in_outputByte1.ptRaise = 0;
					break;
				case DRY2:
					fixtureMotor = 1;
					break;
				case D2STOP:
					paperTowelMotor = 0;
						u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 0;
					u_outputByte1.bits_in_outputByte1.ptRaise = 1;
					u_outputByte1.bits_in_outputByte1.ptLower = 0;
					break;
				case RAISEL2:
					fixtureMotor = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					break;
				case FIXH:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;
					fixtureMotor = 1;
					break;
				case LOWERL3:
					fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					break;
				case UNLOAD:
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 0;
					plateLoadMotor = 1;
					break;
				case END:
					plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
					if(print35 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("END OF CYCLE");
						print35 = 0;
					}
					break;
			}





				/*if( ((counter - counterRef) % (totallength1) ) < (steplength1) && plateLoadMotor)  //check if it is in the right period of the loop to send high
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 1; // set bit 0
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 0; // set bit 0
				}

				if( ((counter - counterRef) % (totallength2) ) < (steplength2) && fixtureMotor)
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength3) ) < (steplength3) && brush1Motor)
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength4) ) < (steplength4) && brush2Motor)
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && paperTowelMotor)
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 0; // set bit 1
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

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, counterRefFive, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor, totallength1, totallength2, totallength3, totallength4, totallength5, steplength1, steplength2, steplength3, steplength4, steplength5);

				counter ++;
				delay_ms(1);
   		 	}
		}


		else if(OrangutanDigital::isInputHigh(IO_D1) && !OrangutanDigital::isInputHigh(IO_D2)){
			OrangutanLCD::gotoXY(7,0);
			OrangutanLCD::print("SECOND B");
			while(state != DONER)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
				if(state != INIT){
					OrangutanLCD::gotoXY(0,1);
					OrangutanLCD::print("STATE ");
					OrangutanLCD::print(state);
				}

				/*if(stateButton == 0 && OrangutanDigital::isInputHigh(IO_D0)){
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
				}*/
				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);

				//state transitions
	
				if(state == INIT && buttonTriggered){
					buttonTriggered = false;
					state ++;
					counterRef = counter;
				}
				if(state == LOAD && /*counter - counterRef > totallength1*plateLoadMotorLoadPlate*/&& u_inputByte0.bits_in_inputByte0.plate == 0){
					state ++;
					counterRef = counter;
				}
				if(state == RAISEL1 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == FIXL && counter - counterRef > totallength2*fixtureMotorHomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == LOWERL1 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == LOWERL2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == MOVEC1 && counter - counterRef > totallength2*fixtureMotorBrush2StepWhole /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state = B2SET;
					counterRef = counter;
				}
				if(state == B2SET && counter - counterRef > mWait){
					state ++;
					counterRef = counter;
				}
				if(state == B2START1 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == CLEAN2_1 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state ++;
					counterRef = counter;
				}
				if(state == B2STOP1 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == CLEAN2_2 && counter - counterRef > totallength2*fixtureMotorHalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush2 == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == B2START2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == CLEAN2_3 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state ++;
					counterRef = counter;
				}
				if(state == B2STOP2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == MOVED1 && counter - counterRef > totallength2*fixtureMotorDry1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry1 == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == D1START && counter - counterRef > kWait){
					state ++;
					counterRef = counter;
				}
				if(state == DRY1 && counter - counterRef > totallength2*fixtureMotorWholePlate){
					state ++;
					counterRef26 = counter;
				}
				if(state == D1STOP && counter - counterRef > totallength2*fixtureMotorDry2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry2 == 0*/){
					state ++;
					counterRefFive = counter;
				}
				if(state == D2START && counter - counterRefFive > mWait){
					state ++;
					counterRef = counter;
				}
				if(state == D2RAISE && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == DRY2 && counter - counterRef > totallength2*fixtureMotorWholePlate){
					state ++;
					counterRef30 = counter;
				}
				if(state == D2STOP && counter - counterRef > totallength2*fixtureMotorLoadBack /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == RAISEL2 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == FIXH && counter - counterRef > totallength2*fixtureMotorHomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureHome == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == LOWERL3 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == UNLOAD && counter - counterRef > totallength1*plateLoadMotorLoadPlate){
					state ++;
					counterRef = counter;
				}
				if(state == END && counter - counterRef > 10){
					state = DONER;
				}


				//state actions
						
				switch (state){
				case INIT:
					if(u_inputByte0.bits_in_inputByte0.fixtureHome == 1){
						u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  //********* 0 is used as fixture backward ?cc?, 1 as forward ?c? **********
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						fixtureMotor = 1;
					}
					else{
						u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
						fixtureMotor = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						if(print0 == 1){
							OrangutanLCD::gotoXY(0,1);
							OrangutanLCD::print("PUSH TO START");
							print0 = 0;
						}
					}
					break;
				case LOAD:
					//OrangutanLCD::clear();
					u_outputByte0.bits_in_outputByte0.ACPower = 0;
					if((counter % 200) < 100){
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
					}
					else{
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					}
					plateLoadMotor = 1;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 1;  //****** dir1 ******
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
					u_outputByte0.bits_in_outputByte0.plateStop = 0;
					break;
				case RAISEL1:
					u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.plateStop = 1;
						plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
					break;
				case FIXL:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ****** dir2 *******
					fixtureMotor = 1;
					break;
				case LOWERL1:
						fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					break;
				case LOWERL2:
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
					break;
				//*********************************************
				case MOVEC1:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
				
					break;
				//*************************** may need to reverse order depending on dry station 1 positioning ***************************
				case B2SET:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 1;
					u_motorByte1.bits_in_motorByte1.brush2MotorDir = 0;  // ******* dir4 *******
					/*if(print15 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 2");
						print15 = 0;
					}*/
					break;
				case B2START1:
					brush2Motor = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
					break;
				case CLEAN2_1:
					fixtureMotor = 1;
					//u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					break;
				case B2STOP1:
					fixtureMotor = 0;
					brush2Motor = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
					break;
				case CLEAN2_2:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case B2START2:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush2MotorDir = 0;  // ******* dir4 *******
					brush2Motor = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
					break;
				case CLEAN2_3:
					fixtureMotor = 1;
					break;
				case B2STOP2:
					brush2Motor = 0;
					fixtureMotor = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
					u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 0;
					break;
				//*************************************************
				case MOVED1:
					//u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					//u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 0;
					break;
				case D1START:
					fixtureMotor = 0;
					u_outputByte1.bits_in_outputByte1.airKnife = 0;
						/*if(print24 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("DRY AIR");
						print24 = 0;
					}*/
					break;
				case DRY1:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case D1STOP:
					u_outputByte1.bits_in_outputByte1.airKnife = 1;
					break;
				case D2START:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 1;
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDir = 0;
					paperTowelMotor = 1;
					break;
				case D2RAISE:
					/*if(print28 == 1){
							OrangutanLCD::clear();
						OrangutanLCD::print("FINAL DRY");
						print28 = 0;
						}*/
					u_outputByte1.bits_in_outputByte1.ptRaise = 0;
					break;
				case DRY2:
					fixtureMotor = 1;
					break;
				case D2STOP:
					paperTowelMotor = 0;
						u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 0;
					u_outputByte1.bits_in_outputByte1.ptRaise = 1;
					u_outputByte1.bits_in_outputByte1.ptLower = 0;
					break;
				case RAISEL2:
					fixtureMotor = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					break;
				case FIXH:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;
					fixtureMotor = 1;
					break;
				case LOWERL3:
					fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					break;
				case UNLOAD:
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 0;
					plateLoadMotor = 1;
					break;
				case END:
					plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
					if(print35 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("END OF CYCLE");
						print35 = 0;
					}
					break;
			}





				/*if( ((counter - counterRef) % (totallength1) ) < (steplength1) && plateLoadMotor)  //check if it is in the right period of the loop to send high
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 1; // set bit 0
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 0; // set bit 0
				}

				if( ((counter - counterRef) % (totallength2) ) < (steplength2) && fixtureMotor)
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength3) ) < (steplength3) && brush1Motor)
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength4) ) < (steplength4) && brush2Motor)
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && paperTowelMotor)
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 0; // set bit 1
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

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, counterRefFive, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor, totallength1, totallength2, totallength3, totallength4, totallength5, steplength1, steplength2, steplength3, steplength4, steplength5);

				counter ++;
				delay_ms(1);
   		 	}
		}


		else if(!OrangutanDigital::isInputHigh(IO_D1) && OrangutanDigital::isInputHigh(IO_D2)){
			OrangutanLCD::gotoXY(7,0);
			OrangutanLCD::print("BOTH B");
			while(state != DONER)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
				if(state != INIT){
					OrangutanLCD::gotoXY(0,1);
					OrangutanLCD::print("STATE ");
					OrangutanLCD::print(state);
				}

				/*if(stateButton == 0 && OrangutanDigital::isInputHigh(IO_D0)){
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
				}*/

				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);

				//state transitions
	
				if(state == INIT && buttonTriggered){
					buttonTriggered = false;
					state ++;
					counterRef = counter;
				}
				if(state == LOAD && /*counter - counterRef > totallength1*plateLoadMotorLoadPlate*/&& u_inputByte0.bits_in_inputByte0.plate == 0){
					state ++;
					counterRef = counter;
				}
				if(state == RAISEL1 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == FIXL && counter - counterRef > totallength2*fixtureMotorHomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == LOWERL1 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == LOWERL2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == MOVEC1 && counter - counterRef > totallength2*fixtureMotorBrush1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == B1SET && counter - counterRef > mWait){
					state ++;
					counterRef = counter;
				}
				if(state == B1START1 && counter - counterRef > pWait){
					state ++;
						counterRef = counter;
				}
				if(state == CLEAN1_1 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state ++;
					counterRef = counter;
				}
				if(state == B1STOP1 && counter - counterRef > pWait){
					state ++;
				}
				if(state == CLEAN1_2 && counter - counterRef > totallength2*fixtureMotorHalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == B1START2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == CLEAN1_3 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state ++;
					counterRef14 = counter;
				}
				if(state == B1STOP2 && counter - counterRef > totallength2*fixtureMotorBrush2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush2*/){
					state ++;
					counterRef = counter;
				}
				if(state == B2SET && counter - counterRef > mWait){
					state ++;
					counterRef = counter;
				}
				if(state == B2START1 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == CLEAN2_1 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state ++;
					counterRef = counter;
				}
				if(state == B2STOP1 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == CLEAN2_2 && counter - counterRef > totallength2*fixtureMotorHalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush2 == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == B2START2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == CLEAN2_3 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state ++;
					counterRef = counter;
				}
				if(state == B2STOP2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == MOVED1 && counter - counterRef > totallength2*fixtureMotorDry1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry1 == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == D1START && counter - counterRef > kWait){
					state ++;
					counterRef = counter;
				}
				if(state == DRY1 && counter - counterRef > totallength2*fixtureMotorWholePlate){
					state ++;
					counterRef26 = counter;
				}
				if(state == D1STOP && counter - counterRef > totallength2*fixtureMotorDry2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry2 == 0*/){
					state ++;
					counterRefFive = counter;
				}
				if(state == D2START && counter - counterRefFive > mWait){
					state ++;
					counterRef = counter;
				}
				if(state == D2RAISE && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == DRY2 && counter - counterRef > totallength2*fixtureMotorWholePlate){
					state ++;
					counterRef30 = counter;
				}
				if(state == D2STOP && counter - counterRef > totallength2*fixtureMotorLoadBack /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == RAISEL2 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == FIXH && counter - counterRef > totallength2*fixtureMotorHomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureHome == 0*/){
					state ++;
					counterRef = counter;
				}
				if(state == LOWERL3 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
				}
				if(state == UNLOAD && counter - counterRef > totallength1*plateLoadMotorLoadPlate){
					state ++;
					counterRef = counter;
				}
				if(state == END && counter - counterRef > 10){
					state = DONER;
				}


				//state actions
						
			switch (state){
				case INIT:
					if(u_inputByte0.bits_in_inputByte0.fixtureHome == 1){
						u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  //********* 0 is used as fixture backward ?cc?, 1 as forward ?c? **********
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						fixtureMotor = 1;
					}
					else{
						u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
						fixtureMotor = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						if(print0 == 1){
							OrangutanLCD::gotoXY(0,1);
							OrangutanLCD::print("PUSH TO START");
							print0 = 0;
						}
					}
					break;
				case LOAD:
					//OrangutanLCD::clear();
					u_outputByte0.bits_in_outputByte0.ACPower = 0;
					if((counter % 200) < 100){
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
					}
					else{
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					}
					plateLoadMotor = 1;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 1;  //****** dir1 ******
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
					u_outputByte0.bits_in_outputByte0.plateStop = 0;
					break;
				case RAISEL1:
					u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.plateStop = 1;
						plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
					break;
				case FIXL:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ****** dir2 *******
					fixtureMotor = 1;
					break;
				case LOWERL1:
						fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					break;
				case LOWERL2:
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
					break;
				//*********************************************
				case MOVEC1:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case B1SET:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 1;
					u_motorByte0.bits_in_motorByte0.brush1MotorDir = 0;  // ******* dir3 *******
					/*if(print6 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 1");
						print6 = 0;
					}*/
					break;
				case B1START1:
					brush1Motor = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
					break;
				case CLEAN1_1:
					fixtureMotor = 1;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					break;
				case B1STOP1:
					fixtureMotor = 0;
					brush1Motor = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
					break;
				case CLEAN1_2:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case B1START2:
					fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.brush1MotorDir = 0;  // ******* dir3 *******
					brush1Motor = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
					break;
				case CLEAN1_3:
					fixtureMotor = 1;
					break;
				case B1STOP2:
					brush1Motor = 0;
					fixtureMotor = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
				
					break;
				//*************************** may need to reverse order depending on dry station 1 positioning ***************************
				case B2SET:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 1;
					u_motorByte1.bits_in_motorByte1.brush2MotorDir = 0;  // ******* dir4 *******
					/*if(print15 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 2");
						print15 = 0;
					}*/
					break;
				case B2START1:
					brush2Motor = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
					break;
				case CLEAN2_1:
					fixtureMotor = 1;
					//u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					break;
				case B2STOP1:
					fixtureMotor = 0;
					brush2Motor = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
					break;
				case CLEAN2_2:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case B2START2:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush2MotorDir = 0;  // ******* dir4 *******
					brush2Motor = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
					break;
				case CLEAN2_3:
					fixtureMotor = 1;
					break;
				case B2STOP2:
					brush2Motor = 0;
					fixtureMotor = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
					u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 0;
					break;
				//*************************************************
				case MOVED1:
					//u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					//u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 0;
					break;
				case D1START:
					fixtureMotor = 0;
					u_outputByte1.bits_in_outputByte1.airKnife = 0;
						/*if(print24 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("DRY AIR");
						print24 = 0;
					}*/
					break;
				case DRY1:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case D1STOP:
					u_outputByte1.bits_in_outputByte1.airKnife = 1;
					break;
				case D2START:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 1;
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDir = 0;
					paperTowelMotor = 1;
					break;
				case D2RAISE:
					/*if(print28 == 1){
							OrangutanLCD::clear();
						OrangutanLCD::print("FINAL DRY");
						print28 = 0;
						}*/
					u_outputByte1.bits_in_outputByte1.ptRaise = 0;
					break;
				case DRY2:
					fixtureMotor = 1;
					break;
				case D2STOP:
					paperTowelMotor = 0;
						u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 0;
					u_outputByte1.bits_in_outputByte1.ptRaise = 1;
					u_outputByte1.bits_in_outputByte1.ptLower = 0;
					break;
				case RAISEL2:
					fixtureMotor = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					break;
				case FIXH:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;
					fixtureMotor = 1;
					break;
				case LOWERL3:
					fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					break;
				case UNLOAD:
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 0;
					plateLoadMotor = 1;
					break;
				case END:
					plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
					if(print35 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("END OF CYCLE");
						print35 = 0;
					}
					break;
			}




				/*if( ((counter - counterRef) % (totallength1) ) < (steplength1) && plateLoadMotor)  //check if it is in the right period of the loop to send high
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 1; // set bit 0
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 0; // set bit 0
				}

				if( ((counter - counterRef) % (totallength2) ) < (steplength2) && fixtureMotor)
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength3) ) < (steplength3) && brush1Motor)
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength4) ) < (steplength4) && brush2Motor)
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && paperTowelMotor)
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 0; // set bit 1
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

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, counterRefFive, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor, totallength1, totallength2, totallength3, totallength4, totallength5, steplength1, steplength2, steplength3, steplength4, steplength5);

				counter ++;
				delay_ms(1);
   		 	}
		}

		else if(OrangutanDigital::isInputHigh(IO_D1) && OrangutanDigital::isInputHigh(IO_D2)){
			OrangutanLCD::gotoXY(7,0);
			OrangutanLCD::print("DEBUG");
			while(state != DONER)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
				if(state != INIT){
					OrangutanLCD::gotoXY(0,1);
					OrangutanLCD::print("STATE ");
					OrangutanLCD::print(state);
				}

				/*if(stateButton == 0 && OrangutanDigital::isInputHigh(IO_D0)){
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
				}*/

				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);

				//state transitions
	
				if(state == INIT && buttonTriggered){
					buttonTriggered = false;
					state = WAIT;
					counterRef = counter;
				}
				if(state == LOAD && /*counter - counterRef > totallength1*plateLoadMotorLoadPlate*/&& u_inputByte0.bits_in_inputByte0.plate == 0){
					state = WAIT;
					counterRef = counter;
				}
				if(state == RAISEL1 && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == FIXL && counter - counterRef > totallength2*fixtureMotorHomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == LOWERL1 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == LOWERL2 && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == MOVEC1 && counter - counterRef > totallength2*fixtureMotorBrush1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == B1SET && counter - counterRef > mWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == B1START1 && counter - counterRef > pWait){
					state = WAIT;
						counterRef = counter;
				}
				if(state == CLEAN1_1 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state = WAIT;
					counterRef = counter;
				}
				if(state == B1STOP1 && counter - counterRef > pWait){
					state = WAIT;
				}
				if(state == CLEAN1_2 && counter - counterRef > totallength2*fixtureMotorHalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == B1START2 && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == CLEAN1_3 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state = WAIT;
					counterRef14 = counter;
				}
				if(state == B1STOP2 && counter - counterRef > totallength2*fixtureMotorBrush2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush2*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == B2SET && counter - counterRef > mWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == B2START1 && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == CLEAN2_1 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state = WAIT;
					counterRef = counter;
				}
				if(state == B2STOP1 && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == CLEAN2_2 && counter - counterRef > totallength2*fixtureMotorHalfPlate /*&& u_inputByte0.bits_in_inputByte0.fixtureBrush2 == 0*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == B2START2 && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == CLEAN2_3 && counter - counterRef > totallength2*fixtureMotorHalfPlate){
					state = WAIT;
					counterRef = counter;
				}
				if(state == B2STOP2 && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == MOVED1 && counter - counterRef > totallength2*fixtureMotorDry1Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry1 == 0*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == D1START && counter - counterRef > kWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == DRY1 && counter - counterRef > totallength2*fixtureMotorWholePlate){
					state = WAIT;
					counterRef26 = counter;
				}
				if(state == D1STOP && counter - counterRef > totallength2*fixtureMotorDry2Step /*&& u_inputByte0.bits_in_inputByte0.fixtureDry2 == 0*/){
					state = WAIT;
					counterRefFive = counter;
				}
				if(state == D2START && counter - counterRefFive > mWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == D2RAISE && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == DRY2 && counter - counterRef > totallength2*fixtureMotorWholePlate){
					state = WAIT;
					counterRef30 = counter;
				}
				if(state == D2STOP && counter - counterRef > totallength2*fixtureMotorLoadBack /*&& u_inputByte0.bits_in_inputByte0.fixturePlate == 0*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == RAISEL2 && counter - counterRef > 1000 /*&& u_inputByte0.bits_in_inputByte0.fixtureLift == 0*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == FIXH && counter - counterRef > totallength2*fixtureMotorHomeFix /*&& u_inputByte0.bits_in_inputByte0.fixtureHome == 0*/){
					state = WAIT;
					counterRef = counter;
				}
				if(state == LOWERL3 && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
				}
				if(state == UNLOAD && counter - counterRef > totallength1*plateLoadMotorLoadPlate){
					state = WAIT;
					counterRef = counter;
				}
				if(state == END && counter - counterRef > 10){
					state = DONER;
				}
				if(state == WAIT && buttonTriggered){
					state = ++last;
				}


				//state actions
						
			switch (state){
				case INIT:
					if(u_inputByte0.bits_in_inputByte0.fixtureHome == 1){
						u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  //********* 0 is used as fixture backward ?cc?, 1 as forward ?c? **********
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						fixtureMotor = 1;
					}
					else{
						u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
						fixtureMotor = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						if(print0 == 1){
							OrangutanLCD::gotoXY(0,1);
							OrangutanLCD::print("PUSH TO START");
							print0 = 0;
						}
					}
					break;
				case LOAD:
					//OrangutanLCD::clear();
					u_outputByte0.bits_in_outputByte0.ACPower = 0;
					if((counter % 200) < 100){
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
					}
					else{
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					}
					plateLoadMotor = 1;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 1;  //****** dir1 ******
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
					u_outputByte0.bits_in_outputByte0.plateStop = 0;
					break;
				case RAISEL1:
					u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.plateStop = 1;
						plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
					break;
				case FIXL:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ****** dir2 *******
					fixtureMotor = 1;
					break;
				case LOWERL1:
						fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					break;
				case LOWERL2:
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 1;
					break;
				//*********************************************
				case MOVEC1:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case B1SET:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 1;
					u_motorByte0.bits_in_motorByte0.brush1MotorDir = 0;  // ******* dir3 *******
					/*if(print6 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 1");
						print6 = 0;
					}*/
					break;
				case B1START1:
					brush1Motor = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
					break;
				case CLEAN1_1:
					fixtureMotor = 1;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					break;
				case B1STOP1:
					fixtureMotor = 0;
					brush1Motor = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
					break;
				case CLEAN1_2:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case B1START2:
					fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.brush1MotorDir = 0;  // ******* dir3 *******
					brush1Motor = 1;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
					break;
				case CLEAN1_3:
					fixtureMotor = 1;
					break;
				case B1STOP2:
					brush1Motor = 0;
					fixtureMotor = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
				
					break;
				//*************************** may need to reverse order depending on dry station 1 positioning ***************************
				case B2SET:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 1;
					u_motorByte1.bits_in_motorByte1.brush2MotorDir = 0;  // ******* dir4 *******
					/*if(print15 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("Clean 2");
						print15 = 0;
					}*/
					break;
				case B2START1:
					brush2Motor = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
					break;
				case CLEAN2_1:
					fixtureMotor = 1;
					//u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					break;
				case B2STOP1:
					fixtureMotor = 0;
					brush2Motor = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
					break;
				case CLEAN2_2:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case B2START2:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush2MotorDir = 0;  // ******* dir4 *******
					brush2Motor = 1;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
					break;
				case CLEAN2_3:
					fixtureMotor = 1;
					break;
				case B2STOP2:
					brush2Motor = 0;
					fixtureMotor = 0;
					u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
					u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 0;
					break;
				//*************************************************
				case MOVED1:
					//u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1;
					//u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 0;
					break;
				case D1START:
					fixtureMotor = 0;
					u_outputByte1.bits_in_outputByte1.airKnife = 0;
						/*if(print24 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("DRY AIR");
						print24 = 0;
					}*/
					break;
				case DRY1:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
					fixtureMotor = 1;
					break;
				case D1STOP:
					u_outputByte1.bits_in_outputByte1.airKnife = 1;
					break;
				case D2START:
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 1;
					u_motorByte1.bits_in_motorByte1.paperTowelMotorDir = 0;
					paperTowelMotor = 1;
					break;
				case D2RAISE:
					/*if(print28 == 1){
							OrangutanLCD::clear();
						OrangutanLCD::print("FINAL DRY");
						print28 = 0;
						}*/
					u_outputByte1.bits_in_outputByte1.ptRaise = 0;
					break;
				case DRY2:
					fixtureMotor = 1;
					break;
				case D2STOP:
					paperTowelMotor = 0;
						u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 0;
					u_outputByte1.bits_in_outputByte1.ptRaise = 1;
					u_outputByte1.bits_in_outputByte1.ptLower = 0;
					break;
				case RAISEL2:
					fixtureMotor = 0;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
					break;
				case FIXH:
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;
					fixtureMotor = 1;
					break;
				case LOWERL3:
					fixtureMotor = 0;
					u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 1;
					u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
					u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
					break;
				case UNLOAD:
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 0;
					plateLoadMotor = 1;
					break;
				case END:
					plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
					if(print35 == 1){
						OrangutanLCD::clear();
						OrangutanLCD::print("END OF CYCLE");
						print35 = 0;
					}
					break;
			}




				/*if( ((counter - counterRef) % (totallength1) ) < (steplength1) && plateLoadMotor)  //check if it is in the right period of the loop to send high
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 1; // set bit 0
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 0; // set bit 0
				}

				if( ((counter - counterRef) % (totallength2) ) < (steplength2) && fixtureMotor)
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength3) ) < (steplength3) && brush1Motor)
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte0.bits_in_motorByte0.brush1MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRef) % (totallength4) ) < (steplength4) && brush2Motor)
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.brush2MotorStep = 0; // set bit 1
				}

				if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && paperTowelMotor)
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 1; // set bit 1
				}
				else
				{
					u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 0; // set bit 1
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

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, counterRefFive, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor, totallength1, totallength2, totallength3, totallength4, totallength5, steplength1, steplength2, steplength3, steplength4, steplength5);

				counter ++;
				delay_ms(1);
   		 	}
		}

		}
		u_motorByte0.bits_in_motorByte0.plateLoadMotorDrop = 0;
		u_motorByte0.bits_in_motorByte0.fixtureMotorDrop = 0;
		u_motorByte1.bits_in_motorByte1.brush1MotorDrop = 0;
		u_motorByte1.bits_in_motorByte1.brush2MotorDrop = 0;
		u_motorByte1.bits_in_motorByte1.paperTowelMotorDrop = 0;
		
		i2c_start(I2C1+I2C_WRITE);
		i2c_write(0x2);
		i2c_write(u_motorByte0.motorByte0);
		i2c_write(u_motorByte1.motorByte1);
		i2c_stop();
	}
	OrangutanLCD::print("END");
	}
}


void motor_and_write(int counter, int counterRef, int counterRefFive, int plateLoadMotor, int fixtureMotor, int brush1Motor, int brush2Motor, int paperTowelMotor, int totallength1, int totallength2, int totallength3, int totallength4, int totallength5, int steplength1, int steplength2, int steplength3, int steplength4, int steplength5)
{
//motor and write
	if( ((counter - counterRef) % (totallength1) ) < (steplength1) && plateLoadMotor)  //check if it is in the right period of the loop to send high
	{
		u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 1; // set bit 0
	}
	else
	{
		u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 0; // set bit 0
	}
		if( ((counter - counterRef) % (totallength2) ) < (steplength2) && fixtureMotor)
	{
		u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 1; // set bit 1
	}
	else
	{
		u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 0; // set bit 1
	}
	if( ((counter - counterRef) % (totallength3) ) < (steplength3) && brush1Motor)
	{
		u_motorByte0.bits_in_motorByte0.brush1MotorStep = 1; // set bit 1
	}
	else
	{
		u_motorByte0.bits_in_motorByte0.brush1MotorStep = 0; // set bit 1
	}

	if( ((counter - counterRef) % (totallength4) ) < (steplength4) && brush2Motor)
	{
		u_motorByte1.bits_in_motorByte1.brush2MotorStep = 1; // set bit 1
	}
	else
	{
		u_motorByte1.bits_in_motorByte1.brush2MotorStep = 0; // set bit 1
	}

	if( ((counter - counterRefFive) % (totallength5) ) < (steplength5) && paperTowelMotor)
	{
		u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 1; // set bit 1
	}
	else
	{
		u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 0; // set bit 1
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


/*int button_debounce(int counter, int *pcounterRefPush, int *pcounterRefRel, int *pstateButton)
{
	
	if(*pstateButton == NONE && OrangutanDigital::isInputHigh(IO_D0)){
		*pcounterRefPush = counter;
		*pstateButton = PRESSED;
	}
	if(*pstateButton == PRESSED){
		if(counter - *pcounterRefPush > 15){
			if(!OrangutanDigital::isInputHigh(IO_D0)){
				*pcounterRefRel = counter;
				*pstateButton = RELEASED;
			}
		}
		else if(!OrangutanDigital::isInputHigh(IO_D0)){
			*pstateButton = NONE;
		}
	}
	if(*pstateButton == RELEASED){
		if(OrangutanDigital::isInputHigh(IO_D0)){
			*pstateButton = PRESSED;
			*pcounterRefPush = counter;
		}
		else if(counter - *pcounterRefRel > 15){
			*pstateButton = NONE;
			return 0;
		}
	}
	return 1;
}*/


bool button_debounce(int counter, int *pstateButton)
{
	static int counterRefPush;
	static int counterRefRel;
	
	if(*pstateButton == NONE && OrangutanDigital::isInputHigh(IO_D0)){
		counterRefPush = counter;
		*pstateButton = PRESSED;
	}
	if(*pstateButton == PRESSED){
		if(counter - counterRefPush > 15){
			if(!OrangutanDigital::isInputHigh(IO_D0)){
				counterRefRel = counter;
				*pstateButton = RELEASED;
			}
		}
		else if(!OrangutanDigital::isInputHigh(IO_D0)){
			*pstateButton = NONE;
		}
	}
	if(*pstateButton == RELEASED){
		if(OrangutanDigital::isInputHigh(IO_D0)){
			*pstateButton = PRESSED;
			counterRefPush = counter;
		}
		else if(counter - counterRefRel > 15){
			*pstateButton = NONE;
			return true;
		}
	}
	return false;
}
