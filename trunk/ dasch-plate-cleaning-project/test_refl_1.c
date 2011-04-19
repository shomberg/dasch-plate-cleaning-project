#include <pololu/orangutan.h>
#include <pololu/qtr.h>


int main()
{
  
  lcd_init_printf();
  //PORTA = 0;
  //PORTA = 0xFF;

  
  clear();
  printf("About to init");
  // initialize your QTR sensors
  //unsigned char qtr_rc_pins[] = {14, 15, 16};
  //qtr_rc_init(qtr_rc_pins, 3, 2000, 255);  // 800 us timeout, no emitter pin
  unsigned char qtr_analog_pins[] = {0, 1, 2, 3, 4, 5, 6, 7};
  qtr_analog_init(qtr_analog_pins, 8, 10, 14); // 10 samples, emitter pin is PC0

  clear();
  printf("waiting for top button");
  unsigned char button = wait_for_button_press(TOP_BUTTON);
  // optional: wait for some input from the user, such as  a button press
  
  clear();
  printf("calibrating");
  // then start calibration phase and move the sensors over both
  // reflectance extremes they will encounter in your application:
  for (int i = 0; i < 50; i++)  // make the calibration take about 5 seconds
  {
    qtr_calibrate(QTR_EMITTERS_ON);
    delay(20);
  }
 
  clear();
  printf("waiting for top button");
  unsigned char button2 = wait_for_button_press(TOP_BUTTON);
  // optional: signal that the calibration phase is now over and wait for further
  // input from the user, such as a button press

  while (1)
  {
    clear();
    printf("getting vals");
	delay_ms(1000);

    unsigned int sensor_values[8];
	unsigned int sensor_values2[8];
	for(int i= 0; i<8; i++)
	{
	  sensor_values2[i] = 7777;
	  sensor_values[i] = 5555;
	}

	//svp_set_mode(SVP_MODE_ANALOG);
    //set_analog_mode(MODE_8_BIT);

	qtr_read(sensor_values, QTR_EMITTERS_ON);
	qtr_read_calibrated(sensor_values2, QTR_EMITTERS_ON);
	for(int i = 0; i<8; i++)
	{
	  clear();
	  //analog_read(i);
	  //printf("A%d: %d", i, analog_read(i));
	  printf("A%d: %d", i, sensor_values[i]);
	  //print((long)sensor_values[i]);
	  printf("    B%d: %d", i, sensor_values2[i]);
	  //print((long)sensor_values2[i]);
	  delay_ms(1000);
	  clear();
	}
	delay_ms(1000);
  }

  return 0;
}
