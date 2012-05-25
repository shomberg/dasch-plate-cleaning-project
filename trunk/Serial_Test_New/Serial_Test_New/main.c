/* Serial_Test_New - an application for the Pololu Orangutan SVP
 *
 * This application uses the Pololu AVR C/C++ Library.  For help, see:
 * -User's guide: http://www.pololu.com/docs/0J20
 * -Command reference: http://www.pololu.com/docs/0J18
 *
 * Created: 5/24/2012 4:51:46 PM
 *  Author: Steven
 */

#include <pololu/orangutan.h>


void serial_print(char msg[]){
	serial_send_blocking(USB_COMM, msg, strlen(msg));
	serial_send_blocking(USB_COMM, "\r\n", 2);
}

int main()
{
	//serial_set_baud_rate(USB_COMM, 9600);

	//delay_ms(5000);

	char r_buffer[32];
	char s_buffer[32];
	unsigned char size = 32;

	//s_buffer[0] = 'm';
	//s_buffer[1] = 'c';
	//s_buffer[2] = 's';
	//s_buffer[3] = 'p';
	//s_buffer[4] = '\r';
	//s_buffer[5] = '\n';

	//memcpy_P(s_buffer, PSTR("Hi there!\r\n"), 11);
	//serial_send_blocking(USB_COMM, "Hi there!\r\n", 11);
	
	serial_print("HERRO");
	
	//serial_print("HERRO AGAIN");

	//while(1){
		//serial_check();
	//}
	//serial_send(USB_COMM, "mcsp is rock\r\n", 14);
}
