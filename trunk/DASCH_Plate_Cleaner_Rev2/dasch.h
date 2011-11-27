/*
DASCH Plate Cleaning Project _ Bob Simcoe, Steven Homberg
http://hea_www.harvard.edu/DASCH/
Runs on Orangutan SVP 1284p board
*/

#define I2C1  0x40     // device address of First PCA9555 chip (Motors)
#define I2C2  0x42     // device address of Second PCA9555 chip (Outputs)
#define I2C3  0x44	   // device address of Third PCA9555 chip (Inputs)



enum maintenance_states
{
	S0 = 0,		//S = sensor
	S1,
	S2,
	S3,
	S4,
	S5,
	S6,
	S7,
	O0_ON,		//Ox = output x
	O0_OFF,
	O1_ON,
	O1_OFF,
	O2_ON,
	O2_OFF,
	O3_ON,
	O3_OFF,
	O4_ON,
	O4_OFF,
	O5_ON,
	O5_OFF,
	O6_ON,
	O6_OFF,
	O7_ON,
	O7_OFF,
	O8_ON,
	O8_OFF,
	O9_ON,
	O9_OFF,
	O10_ON,
	O10_OFF,
	O11_ON,
	O11_OFF,
	M1_F,		//Mx = motor x
	M1_B,
	M2_F,
	M2_B,
	M3_F,
	M3_B,
	M4_F,
	M4_B,
	M5_F,
	M5_B,
};

enum run_states
{
 	INIT = 0,	//Initialize fixture and prepare for cycle
	LOAD, 		//Load plate sequence
	RAISEL1,		//Raise fixture lifts
	FIXL,		//Move fixture onto lift
	LOWERL1,	//Let lifts down
	LOWERL2,	//Drive lifts down
				
	MOVEC1,		//Move fixture to first cleaning station
	B1SET,		//Set up brush 1 motor
	B1START1,	//Start brush 1 motor and raise brush 1
	CLEAN1_1,	//Clean 1st half of the plate with the first brush
	B1STOP1,	//Lower brush, turn off motor
	CLEAN1_2,	//Return to center of 1st cleaning station
	B1START2,	//Start brush 1 motor and raise brush 1
	CLEAN1_3,	//Clean 2nd half of the plate with the first brush
	B1STOP2,	//Lower brush, turn off motor
				
	MOVEC2,		//Move fixture to second cleaning station
	B2SET,		//Set up brush 2 motor
	B2START1,	//Start brush 2 motor and raise brush 2
	CLEAN2_1,	//Clean 1st half of the plate with the second brush
	B2STOP1,	//Lower brush, turn off motor
	CLEAN2_2,	//Return to center of 2nd cleaning station
	B2START2,	//Start brush 2 motor and raise brush 2
	CLEAN2_3,	//Clean 2nd half of the plate with the second brush
	B2STOP2,	//Lower brush, turn off motor
				
	MOVED1,		//Move fixture to first drying station
	D1START,	//Activate air knife
	DRY1,		//Move plate across air knife
	D1STOP,		//Deactivate air knife, continue to next drying station
				
	D2START,	//Activate paper towel roller
	D2RAISE,	//Raise paper towel roller
	DRY2,		//Move plate across paper towel roller
	D2STOP,		//Deactivate paper towel roller, lower paper towel roller, return to lift
	RAISEL2,	//Raise lift, ?activate pressure in fixture?
	FIXH,		//Home fixture
	LOWERL3,	//Lower lift
	UNLOAD,		//Return plate to loading area
	END			//End of cycle
};


union u_motorByte0_tag{
	struct {
		unsigned int m1Step : 1;
		unsigned int m1Dir : 1;
		unsigned int m1Drop : 1;
		unsigned int m2Step : 1;
		unsigned int m2Dir : 1;
		unsigned int m2Drop : 1;
		unsigned int m3Step : 1;
		unsigned int m3Dir : 1;
	} bits_in_motorByte0;
	char motorByte0;
} u_motorByte0;


union u_motorByte1_tag{
	struct {
		unsigned int m3Drop : 1;
		unsigned int m4Step : 1;
		unsigned int m4Dir : 1;
		unsigned int m4Drop : 1;
		unsigned int m5Step : 1;
		unsigned int m5Dir : 1;
		unsigned int m5Drop : 1;
		unsigned int mNull17 : 1;
	} bits_in_motorByte1;
	char motorByte1;
} u_motorByte1;

union u_outputByte0_tag{
	struct {
		unsigned int ACPower : 1;
		unsigned int blowerPulse : 1;
		unsigned int plateStop : 1;
		unsigned int raiseFixture : 1;
		unsigned int lowerFixture : 1;
		unsigned int brush1Raise : 1;
		unsigned int brush1Lower : 1;
		unsigned int brush2Raise : 1;
	} bits_in_outputByte0;
	char outputByte0;
} u_outputByte0;


union u_outputByte1_tag{
	struct {
		unsigned int brush2Lower : 1;
		unsigned int ptRaise : 1;
		unsigned int ptLower : 1;
		unsigned int airKnife : 1;
		unsigned int oNull14 : 1;
		unsigned int oNull15 : 1;
		unsigned int oNull16 : 1;
		unsigned int oNull17 : 1;
	} bits_in_outputByte1;
	char outputByte1;
} u_outputByte1;

union u_inputByte0_tag{
	struct {
		unsigned int plate : 1;
		unsigned int fixtureLift : 1;
		unsigned int fixtureHome : 1;
		unsigned int fixturePlate : 1;
		unsigned int fixtureBrush1 : 1;
		unsigned int fixtureBrush2 : 1;
		unsigned int fixtureDry1 : 1;
		unsigned int fixtureDry2 : 1;
	} bits_in_inputByte0;
	char inputByte0;
} u_inputByte0;


union u_inputByte1_tag{
	struct {
		unsigned int iNull10 : 1;
		unsigned int iNull11 : 1;
		unsigned int iNull12 : 1;
		unsigned int iNull13 : 1;
		unsigned int iNull14 : 1;
		unsigned int iNull15 : 1;
		unsigned int iNull16 : 1;
		unsigned int iNull17 : 1;
	} bits_in_inputByte1;
	char inputByte1;
} u_inputByte1;

int steplength1 = 1, steplength2 = 1, steplength3 = 1, steplength4 = 1, steplength5 = 1;
int totallength1 = 2, totallength2 = 2;
int totallength3 = 2, totallength4 = 2, totallength5 = 2;
