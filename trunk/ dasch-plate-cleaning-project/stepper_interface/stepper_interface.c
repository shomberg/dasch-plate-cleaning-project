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

  
  
  
unsigned char r_pos = 0;

char check_for_new_bytes_received();

int main()   
{   

  OrangutanSerial::setBaudRate(USB_COMM, 115200);
  char r_buffer[6];

  serial_receive_ring(USB_COMM, r_buffer, sizeof(r_buffer));

  OrangutanLCD::clear();
  OrangutanSerial::send(USB_COMM, PSTR("Hello, Bob!\r\n"), 13);
  delay_ms(500);
  
  
  
  OrangutanSerial::send(USB_COMM, PSTR("Choose Motor Direction: F = forward, B = backward\n\r\n"), 52);
  
  char dir = ''; 

  while(serial_get_received_bytes() != receive_buffer_position)
  {
    dir = check_for_new_bytes_received();
  }
  
  if(dir == 'f')
    OrangutanDigital::setOutput(IO_C1, LOW);//set direction (DIR pin)
  else if(dir == 'b')
    OrangutanDigital::setOutput(IO_C1, HIGH);//set direction (DIR pin)
  else OrangutanLCD::print("ERR");

  delay_ms(500);

  
  
  OrangutanSerial::send(USB_COMM, PSTR("Input Motor Delay (ms): Integer 2-1000\n\r\n"), 41);
  
  int delay_time = 0; 
  int dt[4]; int i=0;
  dt[0] = 0; dt[1] = 0; dt[2] = 0; dt[3] = 0;

  while(serial_get_received_bytes() != receive_buffer_position)
  {
    dt[i] = check_for_new_bytes_received();
	i++;
  }
  
  delay_time = dt[0] * 1000 + dt[1] * 100 + dt[2] * 10 + dt[3];
  
  

  OrangutanSerial::send(USB_COMM, PSTR("Input Time (s): Integer 1-9999\n\r\n"), 33);
  
  int run_time = 2000; // set to received value
  int rt[4]; int i=0;
  rt[0] = 0; rt[1] = 0; rt[2] = 0; rt[3] = 0;

  while(serial_get_received_bytes() != receive_buffer_position)
  {
    rt[i] = check_for_new_bytes_received();
	i++;
  }

  run_time = rt[0] * 1000 + rt[1] * 100 + rt[2] * 10 + rt[3];

  for(int a=0; a<(run_time*1000.0)/(2+delay_time); a++)
  {
    //OrangutanPushbuttons::waitForButton(MIDDLE_BUTTON);

	OrangutanDigital::setOutput(IO_C0, HIGH);
	delay_ms(2);

	OrangutanDigital::setOutput(IO_C0, LOW);
	delay_ms(delay_time);
  }



    //while(!OrangutanSerial::sendBufferEmpty(USB_COMM))
    //{
	  //OrangutanSerial::check();
    //}
  }

  
  return 0;
}  

char check_for_new_bytes_received()
{
  // Increment receive_buffer_position, but wrap around when it gets to
  // the end of the buffer.
  if (receive_buffer_position == sizeof(receive_buffer)-1)
  {
    receive_buffer_position = 0;
  }
  else
  {
    receive_buffer_position++;
  }

  // Process the new byte that has just been received.
  return r_buffer[r_pos];
}
