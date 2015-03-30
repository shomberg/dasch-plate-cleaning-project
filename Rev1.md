//as of yet, untested--need to solder sensor -BSH


#include <pololu/orangutan.h>

int main()
{
> clear();
> print("About to init");
> // initialize your QTR sensors
> //unsigned char qtr\_rc\_pins[.md](.md) = {14, 15, 16};
> //qtr\_rc\_init(qtr\_rc\_pins, 3, 2000, 255);  // 800 us timeout, no emitter pin
> int qtr\_analog\_pins[.md](.md) = {0, 1, 2};
> qtr\_analog\_init(qtr\_analog\_pins, 3, 10, 14); // 10 samples, emitter pin is PC0

> clear();
> print("waiting for top button");
> unsigned char button = wait\_for\_button\_press(TOP\_BUTTON);
> // optional: wait for some input from the user, such as  a button press

> clear();
> print("about to calibrate");
> // then start calibration phase and move the sensors over both
> // reflectance extremes they will encounter in your application:
> for (int i = 0; i < 250; i++)  // make the calibration take about 5 seconds
> {
> > qtr\_calibrate(QTR\_EMITTERS\_ON);
> > delay(20);

> }

> clear();
> print("waiting for top button");
> unsigned char button2 = wait\_for\_button\_press(TOP\_BUTTON);
> // optional: signal that the calibration phase is now over and wait for further
> // input from the user, such as a button press

> while (1)
> {
> > clear();
> > print("getting sensor vals");


> unsigned int sensor\_values[8](8.md);
> unsigned int sensor\_values2[8](8.md);
> qtr\_read(sensor\_values, QTR\_EMITTERS\_ON);
> qtr\_read\_calibrated(sensor\_values2, QTR\_EMITTERS\_ON);
> for(int i = 0; i<8; i++)
> {
> > print(sensor\_values[i](i.md));
> > print(sensor\_values2[i](i.md));
> > delay\_ms(200);
> > clear();

> }
> > }


> return 0;
}