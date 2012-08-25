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

#include "dasch.h"
#include <pololu/orangutan.h>
#include <i2cmaster.h>
#include <stdlib.h>
#include <stdbool.h>

/*void * operator new(size_t size); 
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
void __cxa_pure_virtual(void) {}; */


  
void motor_and_write(int counter, int counterRef, int plateLoadMotor, int fixtureMotor, int brush1Motor, int brush2Motor, int paperTowelMotor);

//int button_debounce(int counter, int *pcounterRefPush, int *pcounterRefRel, int *pstateButton);

bool button_debounce(int counter, int *pstateButton);

bool init_trans(int state, bool buttonTriggered, int counter, int counterRef);

bool firstB_trans(int state, int counter, int counterRef);

bool secondB_trans(int state, int counter, int counterRef);

bool dry_trans(int state, int counter, int counterRef);

void init_action(int state, int counter, int *pplateLoadMotor, int *pfixtureMotor, int *pbrush1Motor, int *pbrush2Motor, int *ppaperTowelMotor, bool *pprint0);

void brush1_action(int state, int counter, int *pplateLoadMotor, int *pfixtureMotor, int *pbrush1Motor, int *pbrush2Motor, int *ppaperTowelMotor);

void brush2_action(int state, int counter, int *pplateLoadMotor, int *pfixtureMotor, int *pbrush1Motor, int *pbrush2Motor, int *ppaperTowelMotor);

void dry_action(int state, int counter, int *pplateLoadMotor, int *pfixtureMotor, int *pbrush1Motor, int *pbrush2Motor, int *ppaperTowelMotor);

void serial_print_string(char msg[]);

void serial_print_int(int a);

int main()   
{
    
    unsigned char ret1, ret2, ret3;		// Used to determine if the I2Cs return properly

					//mx holds whether motor x should move: 0 = no, 1 = yes
	int plateLoadMotor = 0;		//plate load motor
	int fixtureMotor = 0;		//fixture motor
	int brush1Motor = 0;		//brush 1 motor
	int brush2Motor = 0;		//brush 2 motor
	int paperTowelMotor = 0; 	//paper towel roller motor
	int state = 0;										//Holds program's current state
	int counter = 0;									//Counts iteration of the loop for timing purposes
	int last = 0;										//In Debug Cycle, holds program's most recent non-waiting state
	
	int lastVal = -1;					//Holds last value for sensor

	//various reference points used by the program to tell how long since an event has occurred
	//the ones with numbers correspond to run mode states, and Five refers to motor 5 (paper towel roller)
	int counterRef = 0;
		
	//hold whether or not statements have been printed yet (numbers refer to run mode states)
	bool print0 = true;
	bool  print35 = true;
		
	//initialize button variables
	int button = 1;				
	int stateButton = NONE;			//holds the state in the button subroutine where the program is currently

	bool buttonTriggered = false;	//buttonTriggered is false if it hasn't been triggered, true if it has;
	bool switched = false;			//switched is false if the submode in maintenance mode wasn't just switched, true otherwise
	
	fixtureMotorSmallHalfPlate *= stepFactor;
	fixtureMotorBigHalfPlate *= stepFactor;
	fixtureMotorDry *= stepFactor;
	plateLoadMotorLoadPlate *= stepFactor;
	plateLoadExtra *= stepFactor;
	fixtureLiftExtra *= stepFactor;
	fixtureLift2Extra *= stepFactor;
	fixtureMotorDry1Extra *= stepFactor;
	fixtureMotorDry2Step *= stepFactor;
    
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
	
	//hold whether or not statements have been printed yet (numbers refer to run mode states)
	print0 = true;
	print35 = true;
	
	//initialize button variables
	button = 1;				//button = 0 means it has been de-bounced, button = 1 means it hasn't
	buttonTriggered = false;
	stateButton = NONE;		//holds the state in the button subroutine where the program is currently
	u_motorByte0.motorByte0 = 0;  // initialize motorByte0
	u_motorByte1.motorByte1 = 128;  // initialize motorByte1              **********1 or 128, depending on order**********
	u_outputByte0.outputByte0 = 255;  // initialize outputByte0
	u_outputByte1.outputByte1 = 255;  // initialize outputByte1
	u_inputByte0.inputByte0 = 0;  // initialize inputByte0
	u_inputByte1.inputByte1 = 0;  // initialize inputByte1
	
	//clear();
	serial_print_string("DASCH CLEANER 2c");
	lcd_goto_xy(0,1);
	serial_print_string("REV: 44");
	delay_ms(2000);
	
	//clear();
	serial_print_string("BEGIN ");
	
	delay_ms(1000);

    i2c_init();                                // init I2C interface
	
    serial_print_string("INIT ");
	
    ret1 = i2c_start(I2C1+I2C_WRITE);       // ret1 holds whether or not I2C1 started properly
	i2c_stop();	
	ret2 = i2c_start(I2C2+I2C_WRITE);       // ret2 holds whether or not I2C2 started properly
	i2c_stop();
	ret3 = i2c_start(I2C3+I2C_WRITE);       // ret3 holds whether or not I2C3 started properly
	i2c_stop();
	
	if (ret1)
		serial_print_string("I2C1 DOWN ");
	else
		serial_print_string("I2C1 UP");
	if (ret2)
		serial_print_string("I2C2 DOWN ");
	else
		serial_print_string("I2C2 UP");
	if (ret3)
		serial_print_string("I2C3 DOWN ");
	else
		serial_print_string("I2C3 UP");
    
	//clear();
	serial_print_string("START ");
	
	if ( ret1 | ret2 | ret3 ) {			// Check if all three I2C devices responded properly
        /* failed to issue start condition(s), possibly no device found */
        
		serial_print_string("I2C BAD ");
		i2c_stop();
		serial_print_string("STOP PROG ");
    }
	else {
		serial_print_string("I2C OK ");
        /* issuing start conditions ok, devices accessible */
		serial_print_string("BINIT ");
		i2c_start(I2C1+I2C_WRITE);		//Configures I2C1 registers as outputs
		i2c_write(0x6);
		i2c_write(0x0);
		i2c_write(0x0);
		i2c_stop();

		i2c_start(I2C1+I2C_WRITE);
		i2c_write(0x2);
		i2c_write(u_motorByte0.motorByte0);
		i2c_write(u_motorByte1.motorByte1);
		i2c_stop();

		i2c_start(I2C2+I2C_WRITE);		//Configures I2C2 registers as outputs
		i2c_write(0x6);
		i2c_write(0x0);
		i2c_write(0x0);
		i2c_stop();

		i2c_start(I2C2+I2C_WRITE);
		i2c_write(0x2);
		i2c_write(u_outputByte0.outputByte0);
		i2c_write(u_outputByte1.outputByte1);
		i2c_stop();

		//Registers default to input, so I2C3 is fine already

		u_motorByte1.bits_in_motorByte1.enableAll = 0;	//Enables all motor drivers after startup
		i2c_start(I2C1+I2C_WRITE);
		i2c_write(0x2);
		i2c_write(u_motorByte0.motorByte0);
		i2c_write(u_motorByte1.motorByte1);
		i2c_stop();


		delay_ms(1000);
		//clear();
		serial_print_string("INPUT RUN/DEBUG");
		int selection = !is_digital_input_high(IO_D3);
			
		//Repeats until user presses and releases button - waiting for user to select a mode
		while(!buttonTriggered){
			if(selection != is_digital_input_high(IO_D3)){
				if(!selection){
					serial_print_string("DEBUG");
				}
				else{
					serial_print_string("RUN");
				}
			}
			selection = is_digital_input_high(IO_D3);
			buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);
			serial_check(); counter++;
			delay_us(delayTimeMicroSeconds);
		}
		buttonTriggered = false;
		counter = 0;
		//clear();


		if(is_digital_input_high(IO_D3))							//Tests if the switch is set to Maintenance Mode
		{
			serial_print_string("DEBUG");
			delay_ms(1000);
			
			//initialize
			bool printVar = true;
			int submode = -1;

			//clear();
			//serial_print_string("INPUT MODE");
			
			//Repeats until user presses and releases button - waiting for user to select a mode
			/*while(!buttonTriggered){
				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);
				counter++;
				delay_us(delayTimeMicroSeconds);
			}*/
			buttonTriggered = false;
			counter = 0;
			//clear();
			

			//Repeats entire maintenance mode cycle until it's finished
			while(state != DONEM){

				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		

				//User presses button for each state transition
				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);
				

				/*Switches mode to whichever is currently selected with submode selection switch
				Modes are:	Input 			--     starts testing each input individually and ends after testing all inputs
							Output 			--     starts testing each output individually and ends after testing all outputs
							Motor			--     starts testing each motor individually and ends after testing all motors
							Multi motor		--     starts testing each the fixture motor with each of the brush motors

				*/
				if(!is_digital_input_high(IO_D1) && !is_digital_input_high(IO_D2) && submode != INPUT){
					state = S0;
					submode = INPUT;
					//clear();
					serial_print_string("INPUTS ");
					lcd_goto_xy(0,1);
					printVar = true;
					switched = true;
				}
				if(is_digital_input_high(IO_D1) && !is_digital_input_high(IO_D2) && submode != OUTPUT){
					state = O0_ON;
					submode = OUTPUT;
					//clear();
					serial_print_string("OUTPUTS");
					lcd_goto_xy(0,1);
					printVar = true;
					switched = true;
				}
				if(!is_digital_input_high(IO_D1) && is_digital_input_high(IO_D2) && submode != MOTOR){
					state = M1_F;
					submode = MOTOR;
					//clear();
					serial_print_string("MOTORS ");
					lcd_goto_xy(0,1);
					printVar = true;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 1;
					switched = true;
				}
				if(is_digital_input_high(IO_D1) && is_digital_input_high(IO_D2) && submode != MULTI){
					state = M2_AND_M3;
					submode = MULTI;
					//clear();
					serial_print_string("MULTI MOTOR");
					lcd_goto_xy(0,1);
					printVar = true;
					u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
					u_motorByte1.bits_in_motorByte1.brush1MotorHighPower = 1;
					switched = true;
				}
				if(switched){
					plateLoadMotor = 0;
					fixtureMotor = 0;
					brush1Motor = 0;
					brush2Motor = 0;
					paperTowelMotor = 0;
					u_outputByte0.outputByte0 = 255;  // reset outputByte0
					u_outputByte1.outputByte1 = 255;  // reset outputByte1
					if(submode != MOTOR && submode != MULTI){
						u_motorByte0.motorByte0 = 0;	  // reset motorByte0
						u_motorByte1.motorByte1 = 0;	  // reset motorByte1
					}
					switched = false;
				}

				//state transitions

				//Has button been pressed?
				if(buttonTriggered) {					//if a state conversion is in order (button press)
					if(state <= S7){				//if it's doing the sensors
						lastVal = -1;
						lcd_goto_xy(0,1);
						buttonTriggered = false;
						state ++;
						counterRef = counter;
						printVar = true;
						if(state == S7 + 1){		//accounts for end of maintenance mode after sensors
							//clear();
							state = DONEM;
						}
					}
					else if(state <= O11_OFF){		//if it's doing the outputs
						if(state % 2 == 0){			//current state is an 'on' state
							lcd_goto_xy(13,1);
							serial_print_string("OFF");
						}
						else{						//current state is an 'off' state
							lcd_goto_xy(0,1);
						}
						buttonTriggered = false;
						state ++;
						counterRef = counter;
						printVar = true;
						if(state == O11_OFF + 1){	//accounts for end of maintenance mode after outputs
							//clear();
							state = DONEM;
						}
					}
					else if(state <= M5_B){							//if it's doing the motors
						buttonTriggered = false;
						state ++;
						counterRef = counter;
						printVar = true;
						if((state - 1) % 2 == 0){	//if the current state is a 'front' state
							lcd_goto_xy(11,1);
							serial_print_string("BACK ");
						}
						else{						//if the current state is a 'back' state
							switch (state) {
								case M1_B + 1:
									lcd_goto_xy(0,1);
									u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 0;
									u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
									break;
								case M2_B + 1:
									lcd_goto_xy(0,1);
									u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 0;
									u_motorByte1.bits_in_motorByte1.brush1MotorHighPower = 1;
									break;
								case M3_B + 1:
									lcd_goto_xy(0,1);
									u_motorByte1.bits_in_motorByte1.brush1MotorHighPower = 0;
									u_motorByte1.bits_in_motorByte1.brush2MotorHighPower = 1;
									break;
								case M4_B + 1:
									lcd_goto_xy(0,1);
									u_motorByte1.bits_in_motorByte1.brush2MotorHighPower = 0;
									u_motorByte1.bits_in_motorByte1.paperTowelMotorHighPower = 1;
									break;
								case M5_B + 1:
									//clear();
									u_motorByte1.bits_in_motorByte1.paperTowelMotorHighPower = 0;
									state = DONEM;
									paperTowelMotor = 0;
									break;
							}
						}
					}
					else{
						buttonTriggered = false;
						state ++;
						counterRef = counter;
						printVar = true;
						switch(state){
							case M2_AND_M3 + 1:
								lcd_goto_xy(0,1);
								u_motorByte1.bits_in_motorByte1.brush1MotorHighPower = 0;
								u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
								u_motorByte1.bits_in_motorByte1.brush2MotorHighPower = 1;
								break;
							case M2_AND_M4 + 1:
								//clear();
								u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 0;
								u_motorByte1.bits_in_motorByte1.brush2MotorHighPower = 0;
								state = DONEM;
								fixtureMotor = 0;
								brush2Motor = 0;
								break;
						}
					}
				}

				//state actions
				switch (state) {
					case S0:
						if(printVar){
							serial_print_string("SENSOR 1");
							printVar = false;
						}
						lcd_goto_xy(15,1);
						if(lastVal !=u_inputByte0.bits_in_inputByte0.plate){
							serial_print_int(u_inputByte0.bits_in_inputByte0.plate);
							lastVal = u_inputByte0.bits_in_inputByte0.plate;
						}							
						break;
					case S1:
						if(printVar){
							serial_print_string("SENSOR 2");
							printVar = false;
						}
						lcd_goto_xy(15,1);
						if(lastVal !=u_inputByte0.bits_in_inputByte0.fixtureHome){
							serial_print_int(u_inputByte0.bits_in_inputByte0.fixtureHome);
							lastVal = u_inputByte0.bits_in_inputByte0.fixtureHome;
						}							
						break;
					case S2:
						if(printVar){
							serial_print_string("SENSOR 3");
							printVar = false;
						}
						lcd_goto_xy(15,1);
						if(lastVal !=u_inputByte0.bits_in_inputByte0.fixtureLift){
							serial_print_int(u_inputByte0.bits_in_inputByte0.fixtureLift);
							lastVal = u_inputByte0.bits_in_inputByte0.fixtureLift;
						}							
						break;
					case S3:
						if(printVar){
							serial_print_string("SENSOR 4");
							printVar = false;
						}
						lcd_goto_xy(15,1);
						if(lastVal !=u_inputByte0.bits_in_inputByte0.fixturePlate){
							serial_print_int(u_inputByte0.bits_in_inputByte0.fixturePlate);
							lastVal = u_inputByte0.bits_in_inputByte0.fixturePlate;
						}							
						break;
					case S4:
						if(printVar){
							serial_print_string("SENSOR 5");
							printVar = false;
						}
						lcd_goto_xy(15,1);
						if(lastVal !=u_inputByte0.bits_in_inputByte0.fixtureBrush1){
							serial_print_int(u_inputByte0.bits_in_inputByte0.fixtureBrush1);
							lastVal = u_inputByte0.bits_in_inputByte0.fixtureBrush1;
						}							
						break;
					case S5:
						if(printVar){
							serial_print_string("SENSOR 6");
							printVar = false;
						}
						lcd_goto_xy(15,1);
						if(lastVal !=u_inputByte0.bits_in_inputByte0.fixtureBrush2){
							serial_print_int(u_inputByte0.bits_in_inputByte0.fixtureBrush2);
							lastVal = u_inputByte0.bits_in_inputByte0.fixtureBrush2;
						}							
						break;
					case S6:
						if(printVar){
							serial_print_string("SENSOR 7");
							printVar = false;
						}
						lcd_goto_xy(15,1);
						if(lastVal !=u_inputByte0.bits_in_inputByte0.fixtureDry1){
							serial_print_int(u_inputByte0.bits_in_inputByte0.fixtureDry1);
							lastVal = u_inputByte0.bits_in_inputByte0.fixtureDry1;
						}							
						break;
					case S7:
						if(printVar){
							serial_print_string("SENSOR 8");
							printVar = false;
						}
						lcd_goto_xy(15,1);
						if(lastVal !=u_inputByte0.bits_in_inputByte0.fixtureDry2){
							serial_print_int(u_inputByte0.bits_in_inputByte0.fixtureDry2);
							lastVal = u_inputByte0.bits_in_inputByte0.fixtureDry2;
						}							
						break;
					case O0_ON:
						if(printVar){
							serial_print_string("AC POWER     ON ");
							printVar = false;
						}
						u_outputByte0.bits_in_outputByte0.ACPower = 0;
						break;
					case O0_OFF:
						u_outputByte0.bits_in_outputByte0.ACPower = 1;
						break;
					case O1_ON:
						if(printVar){
							serial_print_string("BLOWER       ON  ");
							printVar = false;
						}
						u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
						break;
					case O1_OFF:
						u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
						break;
					case O2_ON:
						if(printVar){
							serial_print_string("PLATE STOP   ON ");
							printVar = false;
						}
						u_outputByte0.bits_in_outputByte0.plateStop = 0;
						break;
					case O2_OFF:
						u_outputByte0.bits_in_outputByte0.plateStop = 1;
						break;
					case O3_ON:
						if(printVar){
							serial_print_string("PLATE RAISE  ON ");
							printVar = false;
						}
						u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
						break;
					case O3_OFF:
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						//u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						break;
					case O4_ON:
						if(printVar){
							serial_print_string("PLATE LOWER  ON ");
							printVar = false;
						}
						u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
						u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
						break;
					case O4_OFF:
						u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
						break;
					case O5_ON:
						if(printVar){
							serial_print_string("BRUSH1 RAISE ON ");
							printVar = false;
						}
						u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
						u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
						break;
					case O5_OFF:
						u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
						//u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
						break;
					case O6_ON:
						if(printVar){
							serial_print_string("BRUSH1 LOWER ON ");
							printVar = false;
						}
						u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
						u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
						break;
					case O6_OFF:
						u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
						break;
					case O7_ON:
						if(printVar){
							serial_print_string("BRUSH2 RAISE ON ");
							printVar = false;
						}
						u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
						u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
						break;
					case O7_OFF:
						u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
						//u_outputByte0.bits_in_outputByte0.brush2Lower = 0;
						break;
					case O8_ON:
						if(printVar){
							serial_print_string("BRUSH2 LOWER ON ");
							printVar = false;
						}
						u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
						u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
						break;
					case O8_OFF:
						u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
						break;
					case O9_ON:
						if(printVar){
							serial_print_string("PAPER RAISE  ON ");
							printVar = false;
						}
						u_outputByte1.bits_in_outputByte1.ptRaise = 0;
						u_outputByte1.bits_in_outputByte1.ptLower = 1;
						break;
					case O9_OFF:
						u_outputByte1.bits_in_outputByte1.ptRaise = 1;
						//u_outputByte1.bits_in_outputByte1.ptLower = 0;
						break;
					case O10_ON:
						if(printVar){
							serial_print_string("PAPER LOWER  ON ");
							printVar = false;
						}
						u_outputByte1.bits_in_outputByte1.ptLower = 0;
						u_outputByte1.bits_in_outputByte1.ptRaise = 1;
						break;
					case O10_OFF:
						u_outputByte1.bits_in_outputByte1.ptLower = 1;
						break;
					case O11_ON:
						if(printVar){
							serial_print_string("AIR KNIFE    ON ");
							printVar = false;
						}
						u_outputByte1.bits_in_outputByte1.airKnife = 0;
						break;
					case O11_OFF:
						u_outputByte1.bits_in_outputByte1.airKnife = 1;
						break;
					case M1_F:
						if(printVar){
							serial_print_string("LOADING    FRONT");
							printVar = false;
						}
						u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 0;
						plateLoadMotor =  1;
						break;
					case M1_B:
						u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 1;
						plateLoadMotor =  1;
						break;
					case M2_F:
						if(printVar){
							serial_print_string("DRIVE      FRONT");
							printVar = false;
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
						if(printVar){
							serial_print_string("BRUSH1     FRONT");
							printVar = false;
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
						if(printVar){
							serial_print_string("BRUSH2     FRONT");
							printVar = false;
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
						if(printVar){
							serial_print_string("PAPER      FRONT");
							printVar = false;
						}
						u_motorByte1.bits_in_motorByte1.paperTowelMotorDir = 0;
						paperTowelMotor =  1;
						brush2Motor = 0;
						break;
					case M5_B:
						u_motorByte1.bits_in_motorByte1.paperTowelMotorDir = 1;
						paperTowelMotor =  1;
						break;
					case M2_AND_M3:
						if(printVar){
							serial_print_string("FIXTURE & BRUSH1");
							printVar = false;
						}
						fixtureMotor = 1;
						brush1Motor = 1;
						break;
					case M2_AND_M4:
						if(printVar){
							serial_print_string("FIXTURE & BRUSH2");
							printVar = false;
						}
						fixtureMotor = 1;
						brush1Motor = 0;
						brush2Motor = 1;
						break;
				}

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor);

				serial_check(); counter++;
				delay_us(delayTimeMicroSeconds);
			}
		}
		else{										//If switch is set to Run Mode
		
		bool printVar = true;

		serial_print_string("NORMAL");
		delay_ms(1000);
		
		//clear();
		serial_print_string("SELECT CYCLE");
		delay_ms(1000);
		int sel1 = !is_digital_input_high(IO_D1);
		int sel2 = !is_digital_input_high(IO_D2);
		//Repeats until user presses and releases button - waiting for user to select a mode
		while(!buttonTriggered){
			if(sel1 != is_digital_input_high(IO_D1) || sel2 != is_digital_input_high(IO_D2)){
				if(!is_digital_input_high(IO_D1) && !is_digital_input_high(IO_D2))
					serial_print_string("FIRST B");
				else if(is_digital_input_high(IO_D1) && !is_digital_input_high(IO_D2))
					serial_print_string("SECOND B");
				else if(!is_digital_input_high(IO_D1) && is_digital_input_high(IO_D2))
					serial_print_string("BOTH B");
				else
					serial_print_string("DEBUG");
			}
			sel1 = is_digital_input_high(IO_D1);
			sel2 = is_digital_input_high(IO_D2);
			buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);
			counter++;
			delay_us(delayTimeMicroSeconds);
		}
		buttonTriggered = false;
		counter = 0;
		//clear();
		serial_print_string("CYCLE:");

		u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;

		if(!is_digital_input_high(IO_D1) && !is_digital_input_high(IO_D2)){
			lcd_goto_xy(7,0);
			serial_print_string("FIRST B");
			fixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			while(state != DONER)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
				if(state != INIT && printVar){
					lcd_goto_xy(6,1);
					serial_print_string("     ");
					lcd_goto_xy(0,1);
					serial_print_string("STATE ");
					serial_print_int(state);
					printVar = false;
				}

				
				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);

				//state transitions
	
				//Init
				if(init_trans(state, buttonTriggered, counter, counterRef)){
					buttonTriggered = false;
					state ++;
					counterRef = counter;
					printVar = true;
				}

				//First Brush
				if(firstB_trans(state, counter, counterRef)){
					state ++;
					if(state != B1STOP1 && state != CLEAN1_3)         // want to print number of steps to reach here
						counterRef = counter;
					printVar = true;
				}

				if(state == B1STOP2 && counter - counterRef > pWait){
					state = MOVED1;
					counterRef = counter;
					printVar = true;
				}
				if(state == MOVED1 /*&& counter - counterRef > totalStepLength2*fixtureMotorDry1StepWhole*/ && u_inputByte0.bits_in_inputByte0.fixtureDry1 == 0){
					state ++;
					counterRef = counter;
					printVar = true;
				}

				//Dry
				if(dry_trans(state, counter, counterRef)){
					state ++;
					counterRef = counter;
					printVar = true;
				}

				if(state == END && counter - counterRef > 10){
					state = DONER;
				}


				//state actions
				init_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor, &print0);
				brush1_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				if(state == B1STOP2){
					brush1Motor = 0;
					fixtureMotor = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
				}
				if(state == MOVED1){
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
					u_motorByte1.bits_in_motorByte1.brush1MotorHighPower = 0;
				}
				dry_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				if(state == END){
					plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 0;
					if(print35 == 1){
						//clear();
						serial_print_string("END OF CYCLE");
						print35 = 0;
					}
				}

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor);

				serial_check(); counter++;
				delay_us(delayTimeMicroSeconds);
   		 	}
		}


		else if(is_digital_input_high(IO_D1) && !is_digital_input_high(IO_D2)){
			lcd_goto_xy(7,0);
			serial_print_string("SECOND B");
			fixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			while(state != DONER)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
				if(state != INIT && printVar){
					lcd_goto_xy(6,1);
					serial_print_string("     ");
					lcd_goto_xy(0,1);
					serial_print_string("STATE ");
					serial_print_int(state);
					printVar = false;
				}

				
				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);

				//state transitions
	
				//Init
				if(init_trans(state, buttonTriggered, counter, counterRef)){
					buttonTriggered = false;
					state ++;
					counterRef = counter;
					printVar = true;
				}

				if(state == MOVEC1 && /*counter - counterRef > totalStepLength2*fixtureMotorBrush2StepWhole &&*/ u_inputByte0.bits_in_inputByte0.fixtureBrush2 == 0){
					state = B2SET1;
					counterRef = counter;
					printVar = true;
				}

				//Brush2
				if(secondB_trans(state, counter, counterRef)){
					state ++;
					counterRef = counter;
					printVar = true;
				}

				//Dry
				if(dry_trans(state, counter, counterRef)){
					state ++;
					counterRef = counter;
					printVar = true;
				}

				if(state == END && counter - counterRef > 10){
					state = DONER;
				}


				//state actions
				
				init_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor, &print0);
				if(state == MOVEC1){
					u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
					fixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
				}		
				brush2_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				dry_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				if(state == END){
					plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 0;
					if(print35 == 1){
						//clear();
						serial_print_string("END OF CYCLE");
						print35 = 0;
					}
				}

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor);

				serial_check(); counter++;
				delay_us(delayTimeMicroSeconds);
   		 	}
		}


		else if(!is_digital_input_high(IO_D1) && is_digital_input_high(IO_D2)){
			lcd_goto_xy(7,0);
			serial_print_string("BOTH B");
			fixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			while(state != DONER)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
				if(state != INIT && printVar){
					lcd_goto_xy(6,1);
					serial_print_string("       ");
					lcd_goto_xy(0,1);
					serial_print_string("STATE ");
					serial_print_int(state);
					printVar = false;
				}

				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);

				//state transitions
	
				//Init
				if(init_trans(state, buttonTriggered, counter, counterRef)){
					buttonTriggered = false;
					state ++;
					counterRef = counter;
					printVar = true;
				}

				
				//First Brush
				if(firstB_trans(state, counter, counterRef)){
					state ++;
					if(state != B1STOP1 && state != CLEAN1_3)
						counterRef = counter;
					printVar = true;
				}
				if(state == B1STOP2 && counter - counterRef > pWait){
					state ++;
					counterRef = counter;
					printVar = true;
				}

				
				//Brush2
				if(secondB_trans(state, counter, counterRef)){
					state ++;
					counterRef = counter;
					printVar = true;
				}

				//Dry
				if(dry_trans(state, counter, counterRef)){
					state ++;
					counterRef = counter;
					printVar = true;
				}

				if(state == END && counter - counterRef > 10){
					state = DONER;
				}


				//state actions
				init_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor, &print0);
				brush1_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				if(state == B1STOP2){
					brush1Motor = 0;
					fixtureMotor = 0;
					u_motorByte1.bits_in_motorByte1.brush1MotorHighPower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
				}
				brush2_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				dry_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				if(state == END){
					plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 0;
					if(print35 == 1){
						//clear();
						serial_print_string("END OF CYCLE");
						print35 = 0;
					}
				}

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor);

				serial_check(); counter++;
				delay_us(delayTimeMicroSeconds);
   		 	}
		}

		else if(is_digital_input_high(IO_D1) && is_digital_input_high(IO_D2)){
			lcd_goto_xy(7,0);
			serial_print_string("DEBUG");
			fixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			while(state != DONER)
			{
				i2c_start(I2C3+I2C_WRITE);						// first start condition (write)
				i2c_write(0x0);								// write command byte
				i2c_start(I2C3+I2C_READ);						// second start condition (read)
				u_inputByte0.inputByte0 = i2c_readAck();				// read first byte and send Ack, requesting more
   		     	u_inputByte1.inputByte1 = i2c_readNak();				// read second byte and send stop condition
  	 	     	i2c_stop();								// set stop conditon = release bus 		
				
				if(state != INIT && printVar){
					lcd_goto_xy(6,1);
					serial_print_string("     ");
					lcd_goto_xy(0,1);
					serial_print_string("STATE ");
					serial_print_int(state);
					printVar = false;
				}

				buttonTriggered = button_debounce(counter, &stateButton);//, &counterRefPush, &counterRefRel, &stateButton);

				//state transitions
	
				//Init
				if(init_trans(state, buttonTriggered, counter, counterRef)){
					buttonTriggered = false;
					state = WAIT;
					counterRef = counter;
					printVar = true;
				}

				//First Brush
				if(firstB_trans(state, counter, counterRef)){
					state = WAIT;
					if(state != B1STOP1 && state != CLEAN1_3)
						counterRef = counter;
					printVar = true;
				}
				if(state == B1STOP2 && counter - counterRef > pWait){
					state = WAIT;
					counterRef = counter;
					printVar = true;
				}

				
				//Brush2
				if(secondB_trans(state, counter, counterRef)){
					state = WAIT;
					counterRef = counter;
					printVar = true;
				}
				
				//Dry
				if(dry_trans(state, counter, counterRef)){
					state = WAIT;
					counterRef = counter;
					printVar = true;
				}

				if(state == END && counter - counterRef > 10){
					state = DONER;
				}
				if(state == WAIT && buttonTriggered){
					state = ++last;
					printVar = true;
				}


				//state actions
						
				if(state == WAIT){
					plateLoadMotor = 0;
					fixtureMotor = 0;
					brush1Motor = 0;
					brush2Motor = 0;
					paperTowelMotor = 0;
				}
				init_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor, &print0);
				brush1_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				if(state == B1STOP2){
					brush1Motor = 0;
					fixtureMotor = 0;
					u_outputByte0.bits_in_outputByte0.brush1Lower = 0;
					u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
				}		
				brush2_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				dry_action(state, counter, &plateLoadMotor, &fixtureMotor, &brush1Motor, &brush2Motor, &paperTowelMotor);
				if(state == END){
					plateLoadMotor = 0;
					u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 0;
					if(print35 == 1){
						//clear();
						serial_print_string("END OF CYCLE");
						print35 = 0;
					}
				}

				//determines which motors need to be sent which signals and writes the outputs and motors to the appropriate I2C expander
				motor_and_write(counter, counterRef, plateLoadMotor, fixtureMotor, brush1Motor, brush2Motor, paperTowelMotor);

				serial_check(); counter++;
				delay_us(delayTimeMicroSeconds);
   		 	}
		}

		}
		u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 0;
		u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 0;
		u_motorByte1.bits_in_motorByte1.brush1MotorHighPower = 0;
		u_motorByte1.bits_in_motorByte1.brush2MotorHighPower = 0;
		u_motorByte1.bits_in_motorByte1.paperTowelMotorHighPower = 0;
		
		i2c_start(I2C1+I2C_WRITE);
		i2c_write(0x2);
		i2c_write(u_motorByte0.motorByte0);
		i2c_write(u_motorByte1.motorByte1);
		i2c_stop();
	}
	serial_print_string("END");
	}
}

void serial_print_string(char msg[]){
	serial_send_blocking(USB_COMM, msg, strlen(msg));
	serial_send_blocking(USB_COMM, "\r\n", 2);
}

void serial_print_int(int a){
	char msg[5];
	itoa(a, msg, 10);
	serial_print_string(msg);
}


void motor_and_write(int counter, int counterRef, int plateLoadMotor, int fixtureMotor, int brush1Motor, int brush2Motor, int paperTowelMotor)
{
//motor and write
	if( ((counter - counterRef) % (totalStepLength1) ) < (highLength1) && plateLoadMotor)  //check if it is in the right period of the loop to send high
	{
		u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 1; // set bit 1
	}
	else
	{
		u_motorByte0.bits_in_motorByte0.plateLoadMotorStep = 0; // set bit 0
	}
		if( ((counter - counterRef) % (totalStepLength2) ) < (highLength2) && fixtureMotor)
	{
		u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 1; // set bit 1
	}
	else
	{
		u_motorByte0.bits_in_motorByte0.fixtureMotorStep = 0; // set bit 0
	}
	if( ((counter - counterRef) % (totalStepLength3) ) < (highLength3) && brush1Motor)
	{
		u_motorByte0.bits_in_motorByte0.brush1MotorStep = 1; // set bit 1
	}
	else
	{
		u_motorByte0.bits_in_motorByte0.brush1MotorStep = 0; // set bit 0
	}

	if( ((counter - counterRef) % (totalStepLength4) ) < (highLength4) && brush2Motor)
	{
		u_motorByte1.bits_in_motorByte1.brush2MotorStep = 1; // set bit 1
	}
	else
	{
		u_motorByte1.bits_in_motorByte1.brush2MotorStep = 0; // set bit 0
	}

	if( ((counter - counterRef) % (totalStepLength5) ) < (highLength5) && paperTowelMotor)
	{
		u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 1; // set bit 1
	}
	else
	{
		u_motorByte1.bits_in_motorByte1.paperTowelMotorStep = 0; // set bit 0
	}

	i2c_start(I2C1+I2C_WRITE);
	i2c_write(0x2);									// write command byte
	i2c_write(u_motorByte0.motorByte0);                       // write first byte of output
	i2c_write(u_motorByte1.motorByte1);                       // write second byte of output
	i2c_stop();                            // set stop condition = release bus

	i2c_start(I2C2+I2C_WRITE);                // why the second write?
	i2c_write(0x2);
	i2c_write(u_outputByte0.outputByte0);
	i2c_write(u_outputByte1.outputByte1);
	i2c_stop();
}

void init_action(int state, int counter, int *pplateLoadMotor, int *pfixtureMotor, int *pbrush1Motor, int *pbrush2Motor, int *ppaperTowelMotor, bool *pprint0){
	switch(state){
		case INIT:
			if(u_inputByte0.bits_in_inputByte0.fixtureHome == 1){
				u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  //********* 0 is used as fixture backward ?cc?, 1 as forward ?c? **********
				//u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
			}
			else{
				u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 0;
				*pfixtureMotor = 0;
				u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
				u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
				if(*pprint0 == 1){
					lcd_goto_xy(0,1);
					serial_print_string("PUSH TO START");
					*pprint0 = 0;
				}
			}
			break;
		case LOAD:
			u_outputByte0.bits_in_outputByte0.ACPower = 0;
			if((counter % 200) < 100){
				u_outputByte0.bits_in_outputByte0.blowerPulse = 0;
			}
			else{
				u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
			}
			*pplateLoadMotor = 1;
			*pfixtureMotor = 0;
			
			u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 0;  //****** dir1 ******
			u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 1;
			u_outputByte0.bits_in_outputByte0.plateStop = 0;
			break;
		case LOADEXTRA:
			*pplateLoadMotor = 1;
			*pfixtureMotor = 0;
			
			u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 0;  //****** dir1 ******
			u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 1;
			u_outputByte0.bits_in_outputByte0.plateStop = 0;
			break;
		case RAISEL1:
			u_outputByte0.bits_in_outputByte0.blowerPulse = 1;
			u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
			u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
			u_outputByte0.bits_in_outputByte0.plateStop = 1;
			*pplateLoadMotor = 0;
			u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 0;
			break;
		case FIXL:
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ****** dir2 *******
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		case FIXLEXTRA:
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ****** dir2 *******
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		case LOWERL1:
			*pfixtureMotor = 0;
			u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 0;
			u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
			break;
		case LOWERL2:
			u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
			u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		//*********************************************
	}
}

void brush1_action(int state, int counter, int *pplateLoadMotor, int *pfixtureMotor, int *pbrush1Motor, int *pbrush2Motor, int *ppaperTowelMotor){
	switch(state){
		case MOVEC1:
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******   
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		case B1SET1:
			*pfixtureMotor = 0;
			*pbrush1Motor = 1;
			//u_outputByte1.bits_in_outputByte1.brush1Pump = 0;	//Pump control
			u_motorByte1.bits_in_motorByte1.brush1MotorHighPower = 1;
			u_motorByte0.bits_in_motorByte0.brush1MotorDir = 1;  // ******* dir3 *******
			break;
		case B1START1:
			*pbrush1Motor = 1;
			//u_outputByte1.bits_in_outputByte1.brush1Pump = 1;	//Pump control
			u_outputByte0.bits_in_outputByte0.brush1Lower = 1;
			u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
			break;
		case CLEAN1_1:
			*pbrush1Motor = 1;
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
			break;
		case B1STOP1:
			*pfixtureMotor = 0;
			*pbrush1Motor = 0;
			u_outputByte0.bits_in_outputByte0.brush1Lower = 0;   //add brush1 wet here?
			u_outputByte0.bits_in_outputByte0.brush1Raise = 1;
			break;
		case CLEAN1_2:
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		case B1SET2:
			*pfixtureMotor = 0;
			*pbrush1Motor = 1;
			//u_outputByte1.bits_in_outputByte1.brush1Pump = 0;	//Pump control
			u_motorByte0.bits_in_motorByte0.brush1MotorDir = 0;  // ******* dir3 *******
			break;
		case B1START2:
			*pfixtureMotor = 0;
			u_motorByte0.bits_in_motorByte0.brush1MotorDir = 0;  // ******* dir3 *******
			//u_outputByte1.bits_in_outputByte1.brush1Pump = 1;	//Pump control
			*pbrush1Motor = 1;
			u_outputByte0.bits_in_outputByte0.brush1Lower = 1;  //add brush1 wet here?
			u_outputByte0.bits_in_outputByte0.brush1Raise = 0;
			break;
		case CLEAN1_3:
			*pbrush1Motor = 1;
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
	}
}

void brush2_action(int state, int counter, int *pplateLoadMotor, int *pfixtureMotor, int *pbrush1Motor, int *pbrush2Motor, int *ppaperTowelMotor){
	switch(state){
		case MOVEC2:
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 ******* change dir here?? and further??
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		case B2SET1:
			*pfixtureMotor = 0;
			*pbrush2Motor = 1;
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;
			//u_outputByte1.bits_in_outputByte1.brush2Pump = 0;	//Pump control
			u_motorByte1.bits_in_motorByte1.brush2MotorHighPower = 1;
			u_motorByte1.bits_in_motorByte1.brush2MotorDir = 1;  // ******* dir4 ******* 
			break;
		case B2START1:
			*pbrush2Motor = 1;
			//u_outputByte1.bits_in_outputByte1.brush2Pump = 1;	//Pump control
			u_outputByte1.bits_in_outputByte1.brush2Lower = 1;  // add brush2 wet here?
			u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
			break;
		case CLEAN2_1:
			*pbrush2Motor = 1;
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		case B2STOP1:
			*pfixtureMotor = 0;
			*pbrush2Motor = 0;
			u_outputByte1.bits_in_outputByte1.brush2Lower = 0;  //add brush2 wet here??
			u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
			break;
		case CLEAN2_2:
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;  // ******* dir2 *******
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		case B2SET2:
			*pfixtureMotor = 0;
			*pbrush2Motor = 1;
			//u_outputByte1.bits_in_outputByte1.brush2Pump = 0;	//Pump control
			u_motorByte1.bits_in_motorByte1.brush2MotorHighPower = 1;
			u_motorByte1.bits_in_motorByte1.brush2MotorDir = 0;  // ******* dir4 *******
			break;
		case B2START2:
			*pfixtureMotor = 0;
			u_motorByte1.bits_in_motorByte1.brush2MotorDir = 0;  // ******* dir4 *******
			//u_outputByte1.bits_in_outputByte1.brush2Pump = 1;	//Pump control
			*pbrush2Motor = 1;
			u_outputByte1.bits_in_outputByte1.brush2Lower = 1;
			u_outputByte0.bits_in_outputByte0.brush2Raise = 0;
			break;
		case CLEAN2_3:
			*pbrush2Motor = 1;
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		case B2STOP2:
			*pbrush2Motor = 0;
			*pfixtureMotor = 0;
			u_outputByte1.bits_in_outputByte1.brush2Lower = 0;
			u_outputByte0.bits_in_outputByte0.brush2Raise = 1;
			u_motorByte1.bits_in_motorByte1.brush2MotorHighPower = 0;
			break;
		//*************************************************
		case MOVED1:
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
	}
}

void dry_action(int state, int counter, int *pplateLoadMotor, int *pfixtureMotor, int *pbrush1Motor, int *pbrush2Motor, int *ppaperTowelMotor){
	switch(state){
		case MOVED1EXTRA:
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 1;
			*pfixtureMotor = 1;
			break;
		case DRY:
			u_outputByte1.bits_in_outputByte1.airKnife = 0;
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;  // ******* dir2 *******
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			u_motorByte1.bits_in_motorByte1.paperTowelMotorHighPower = 1;
			u_motorByte1.bits_in_motorByte1.paperTowelMotorDir = 0;
			u_outputByte1.bits_in_outputByte1.ptRaise = 0;
			*ppaperTowelMotor = 1;
			break;
		case DSTOP:
			u_outputByte1.bits_in_outputByte1.airKnife = 1;
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			*ppaperTowelMotor = 0;
			u_motorByte1.bits_in_motorByte1.paperTowelMotorHighPower = 0;
			u_outputByte1.bits_in_outputByte1.ptRaise = 1;
			u_outputByte1.bits_in_outputByte1.ptLower = 0;
			break;
		case FIXL2EXTRA:
			u_outputByte1.bits_in_outputByte1.ptLower = 1;
			*pfixtureMotor = 1;
			u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			break;
		case RAISEL2:
			*pfixtureMotor = 0;
			u_outputByte0.bits_in_outputByte0.raiseFixture = 0;
			u_outputByte0.bits_in_outputByte0.lowerFixture = 1;
			break;
		case FIXH:
			u_motorByte0.bits_in_motorByte0.fixtureMotorDir = 0;
			*pfixtureMotor = 1; u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 1;
			u_motorByte0.bits_in_motorByte0.plateLoadMotorHighPower = 1;
			break;
		case LOWERL3:
			u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
			u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
			break;
		case UNLOAD:
			u_outputByte0.bits_in_outputByte0.raiseFixture = 1;
			u_outputByte0.bits_in_outputByte0.lowerFixture = 0;
			if(u_inputByte0.bits_in_inputByte0.fixtureHome == 0){
				u_motorByte0.bits_in_motorByte0.fixtureMotorHighPower = 0;
				*pfixtureMotor = 0;
			}
			u_motorByte0.bits_in_motorByte0.plateLoadMotorDir = 1;
			*pplateLoadMotor = 1;
			break;
	}
}

bool button_debounce(int counter, int *pstateButton)
{
	static int counterRefPush;
	static int counterRefRel;
	
	if(*pstateButton == NONE && is_digital_input_high(IO_D0)){
		counterRefPush = counter;
		*pstateButton = PRESSED;
	}
	if(*pstateButton == PRESSED){
		if(counter - counterRefPush > 15*1000/delayTimeMicroSeconds){
			if(!is_digital_input_high(IO_D0)){
				counterRefRel = counter;
				*pstateButton = RELEASED;
			}
		}
		else if(!is_digital_input_high(IO_D0)){
			*pstateButton = NONE;
		}
	}
	if(*pstateButton == RELEASED){
		if(is_digital_input_high(IO_D0)){
			*pstateButton = PRESSED;
			counterRefPush = counter;
		}
		else if(counter - counterRefRel > 15*1000/delayTimeMicroSeconds){
			*pstateButton = NONE;
			return true;
		}
	}
	return false;
}

bool init_trans(int state, bool buttonTriggered, int counter, int counterRef){
	return((state == INIT && buttonTriggered)||
	(state == LOAD && /*counter - counterRef > totalStepLength1*plateLoadMotorLoadPlate&&*/ u_inputByte0.bits_in_inputByte0.plate == 0)||
	(state == LOADEXTRA && counter - counterRef > totalStepLength1*plateLoadExtra)||
	(state == RAISEL1 && counter - counterRef > pWait)||
	(state == FIXL && /*counter - counterRef > totalStepLength2*fixtureMotorHomeFix &&*/ u_inputByte0.bits_in_inputByte0.fixtureLift == 0)||
	(state == FIXLEXTRA && counter - counterRef > totalStepLength2*fixtureLiftExtra)||
	(state == LOWERL1 && /*counter - counterRef > 1000 &&*/ u_inputByte0.bits_in_inputByte0.fixturePlate == 0)||
	(state == LOWERL2 && counter - counterRef > pWait));
}

bool firstB_trans(int state, int counter, int counterRef){
	//Had references to counterRef14...not used in any transitions, though...unecessary?  replaced with counterRef - don't know how it will work
	return((state == MOVEC1 && /*counter - counterRef > totalStepLength2*fixtureMotorBrush1Step &&*/ u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0)||
	(state == B1SET1 && counter - counterRef > mWait)||
	(state == B1START1 && counter - counterRef > pWait)||
	(state == CLEAN1_1 && counter - counterRef > totalStepLength2*fixtureMotorSmallHalfPlate)||
	(state == B1STOP1 && counter - counterRef > pWait)||
	(state == CLEAN1_2 && /*counter - counterRef > totalStepLength2*fixtureMotorHalfPlate &&*/ u_inputByte0.bits_in_inputByte0.fixtureBrush1 == 0)||
	(state == B1SET2 && counter - counterRef > mWait)||
	(state == B1START2 && counter - counterRef > pWait)||
	(state == CLEAN1_3 && counter - counterRef > totalStepLength2*fixtureMotorBigHalfPlate));
}

bool secondB_trans(int state, int counter, int counterRef){
	return((state == MOVEC2 && /*counter - counterRef > totalStepLength2*fixtureMotorBrush2Step &&*/ u_inputByte0.bits_in_inputByte0.fixtureBrush2 == 0)||
	(state == B2SET1 && counter - counterRef > mWait)||
	(state == B2START1 && counter - counterRef > pWait)||
	(state == CLEAN2_1 && counter - counterRef > totalStepLength2*fixtureMotorBigHalfPlate)||
	(state == B2STOP1 && counter - counterRef > pWait)||
	(state == CLEAN2_2 && /*counter - counterRef > totalStepLength2*fixtureMotorHalfPlate &&*/ u_inputByte0.bits_in_inputByte0.fixtureBrush2 == 0)||
	(state == B2SET2 && counter - counterRef > mWait)||
	(state == B2START2 && counter - counterRef > pWait)||
	(state == CLEAN2_3 && counter - counterRef > totalStepLength2*fixtureMotorSmallHalfPlate)||
	(state == B2STOP2 && counter - counterRef > pWait)||
	(state == MOVED1 && /*counter - counterRef > totalStepLength2*fixtureMotorDry1Step &&*/ u_inputByte0.bits_in_inputByte0.fixtureDry1 == 0));
}

bool dry_trans(int state, int counter, int counterRef){
	//Had references to counterRef26, 30...not used in any transitions, though...unecessary?  replaced with counterRef - don't know how it will work
	return((state == MOVED1EXTRA && counter - counterRef > fixtureMotorDry1Extra)||
	(state == DRY && counter - counterRef > totalStepLength2*fixtureMotorDry)||
	(state == DSTOP && /*counter - counterRef > totalStepLength2*fixtureMotorLoadBack &&*/ u_inputByte0.bits_in_inputByte0.fixturePlate == 0)||
	(state == FIXL2EXTRA && counter - counterRef > totalStepLength2*fixtureLift2Extra)||
	(state == RAISEL2 && /*counter - counterRef > 1000 &&*/ u_inputByte0.bits_in_inputByte0.fixtureLift == 0)||
	(state == FIXH && counter - counterRef > 200)||
	(state == LOWERL3 && counter - counterRef > 500)||
	(state == UNLOAD && counter - counterRef > totalStepLength1*plateLoadMotorLoadPlate));
}
