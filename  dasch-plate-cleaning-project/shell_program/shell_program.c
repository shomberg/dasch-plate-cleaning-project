#include <pololu/orangutan>
#include <pololu/PololuQTRSensors.h>


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



const int PNEUMATIC_DELAY = 300;
const int AIR_PUFF_OUTPUT = IO_C0;
const int VACUUM_OUTPUT = IO_C1;
const int PNEUMATICS_OUTPUT = IO_C2;
const int DIR_PIN = IO_C3;
const int STEPPER_PIN = IO_C4;
const int START_BUTTON = TOP_BUTTON;
const int OVERRIDE_BUTTON = MIDDLE_BUTTON;
const int REDO_BUTTON = BOTTOM_BUTTON;
const int NUM_STEPS = 400;
const int NUM_PANELS = 6;

int sensor_ports[6] = {1, 2, 3, 4, 5, 6};



int load_plate();
int air_clean();
int position_plate();
int lower_tray();
int check_plate();
int vacuum_seal();
int step_forward(int stps);
int step_backward(int stps);
int switch_to_backward();
int switch_to_forward();
int turn_on_brush(int p);
int turn_off_brush(int p);
int turn_on_razor();
int turn_off_razor();
int unload_plate();
int undo_vacuum_seal();
int eject_plate();
int raise_tray();
bool at_end();
bool at_front();
bool at_end(int s);
bool at_front(int s);


  
int main()   
{
  int steps = 0;
  
  switch_to_forward();  


  load_plate();

  OrangutanPushbuttons::waitForButton(START_BUTTON); //the top button on the Orangutan

  while(!at_end(steps))
  {
    step_forward(steps);
	steps++;
  }

  switch_to_backward();
  steps = 0;

  while(!at_front(steps))
  {
    step_backward(steps);
	steps++;
  }

  unload_plate();

  return 0;
}  



int load_plate()
{
  position_plate();
  air_clean();
  check_plate();
  lower_tray();
  vacuum_seal();
  
  return 0;
}

int air_clean()
{
  //send output to puff air
  OrangutanDigital::setOutput(AIR_PUFF_OUTPUT, HIGH);
  delay_ms(2);

  OrangutanDigital::setOutput(AIR_PUFF_OUTPUT, LOW);
  delay_ms(2);

  return 0;
}

int position_plate()
{
  //motors? pneumatics? unknown as of now
  
  return 0;
}

int lower_tray()
{
  //send output to pneumatics
  OrangutanDigital::setOutput(PNEUMATICS_OUTPUT, HIGH);
  delay_ms(2);

  OrangutanDigital::setOutput(PNEUMATICS_OUTPUT, LOW);
  delay_ms(2);

  delay_ms(PNEUMATIC_DELAY);
  return 0;
}

int check_plate()
{
  //get sensor input


  //analyze sensor data
  int correct = 1;  //or 0, based on analysis


  if(correct == 1)
    return 0;
  else
  {
    OrangutanLEDs::red(HIGH);
	char button = OrangutanPushbuttons::waitForButton(OVERRIDE_BUTTON || REDO_BUTTON);
	if(button == OVERRIDE_BUTTON)
	  return 0;
    else
	{
	  eject_plate();//will need to change to "unposition plate" or some such if ejection is on opposite end
	  return 1;
	}
  }
}

int vacuum_seal()
{
  //send signal to compressor
  OrangutanDigital::setOutput(VACUUM_OUTPUT, HIGH);
  delay_ms(2);

  OrangutanDigital::setOutput(VACUUM_OUTPUT, LOW);
  delay_ms(2);

  return 0;
}




int step_forward(int stps)
{
  //send pulse to step once
  OrangutanDigital::setOutput(STEPPER_PIN, HIGH);
  delay_ms(2);

  OrangutanDigital::setOutput(STEPPER_PIN, LOW);
  delay_ms(2);

  int pnls = stps/NUM_STEPS;

  if(pnls == 1 || pnls == 2 || pnls ==3) //for example
  {
	if(OrangutanAnalog::read(sensor_ports[1]) == 1 || OrangutanAnalog::read(sensor_ports[2]) == 1 || OrangutanAnalog::read(sensor_ports[3]) == 1)
      turn_on_brush(pnls);
    //else error code
  }
  else turn_off_brush(pnls);

  if(pnls == 4)
  {
    if(OrangutanAnalog::read(sensor_ports[4]) == 1)
	  turn_on_razor();
    //else error code
  }
  else turn_off_razor();
    

  return 0;
}

int step_backward(int stps)
{
  //send pulse to step once
  OrangutanDigital::setOutput(STEPPER_PIN, HIGH);
  delay_ms(2);

  OrangutanDigital::setOutput(STEPPER_PIN, LOW);
  delay_ms(2);
  
  int pnls = NUM_STEPS * NUM_PANELS - stps/NUM_STEPS;

  if(pnls == 1 || pnls == 2 || pnls ==3) //for example
  {
	if(OrangutanAnalog::read(sensor_ports[1]) == 1 || OrangutanAnalog::read(sensor_ports[2]) == 1 || OrangutanAnalog::read(sensor_ports[3]) == 1)
      turn_on_brush(pnls);
    //else error code
  }
  else turn_off_brush(pnls);

  if(pnls == 4)
  {
    if(OrangutanAnalog::read(sensor_ports[4]) == 1)
	  turn_on_razor();
    //else error code
  }
  else turn_off_razor();

  return 0;
}

int switch_to_backward()
{
  OrangutanDigital::setOutput(DIR_PIN, LOW);
  return 0;
}

int switch_to_forward()
{
  OrangutanDigital::setOutput(DIR_PIN, HIGH);
  return 0;
}


int turn_on_brush(int p)
{
  //OrangutanDigital::setOutput(brush#p,HIGH);
  return 0;
}

int turn_off_brush(int p)
{
  //OrangutanDigital::setOutput(brush#p, LOW);
  return 0;
}

int turn_on_razor()
{
  //OrangutanDigital::setOutput(razor, HIGH); or whatever output is appropriate
  return 0;
}

int turn_off_razor()
{
  //OrangutanDigital::setOutput(razor, LOW); or whatever output is appropriate
  return 0;
}




int unload_plate()
{
  undo_vacuum_seal();
  raise_tray();
  eject_plate();
  return 0;
}

int undo_vacuum_seal()
{
  //send signal to compressor
  OrangutanDigital::setOutput(VACUUM_OUTPUT, HIGH);
  delay_ms(2);

  OrangutanDigital::setOutput(VACUUM_OUTPUT, LOW);
  delay_ms(2);

  return 0;
}

int raise_tray()
{
  //send signal to pneumatics
  OrangutanDigital::setOutput(PNEUMATICS_OUTPUT, HIGH);
  delay_ms(2);

  OrangutanDigital::setOutput(PNEUMATICS_OUTPUT, LOW);
  delay_ms(2);

  delay_ms(PNEUMATIC_DELAY);
  return 0;
}

int eject_plate()
{
  //do what is necessary to eject plate--motors, pneumatics?
  return 0;
}




bool at_end()
{
  //check steps?  check sensors?
  return false;
}

bool at_front()
{
  //check steps?  check sensors?
  return false;
}


bool at_end(int s)
{
  if(s == NUM_STEPS * NUM_PANELS)
    return true;
  else return false;
}

bool at_front(int s)
{
  if(s == NUM_STEPS * NUM_PANELS)
    return true;
  else return false;
}

/*
TODO:
interrupts
sensor stuff
when details known, fill in
*/
