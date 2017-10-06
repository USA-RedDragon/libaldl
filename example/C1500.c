#include <libaldl.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define DEBUG

#ifdef DEBUG
void printHexString(byte* str, int length);
#endif

void printErrorCodes(byte bits, char* errors[]);
int bitValue(byte num, int bit);

//Mode 0 command
byte ecm_mode0[] = {0xF4, 0x56, 0x00, 0xB6};

//Mode 1 command
byte ecm_mode1[] = {0xF4, 0x57, 0x01, 0x00, 0xB4};

//Mode 0 data
byte ecm_mode0_data[4];

//Mode 1 data
byte ecm_mode1_data[72];

int sigEnd=0;

//150 ms scan time
struct timespec scanTime = {0, 150 * 1000000};

void sig_handler(int signo) {
	if (signo == SIGINT) {
		printf("Shutting Down...\n");
		sigEnd=1;
	}
}

int main(int argc, char **argv) {
	if (signal(SIGINT, sig_handler) == SIG_ERR) {
		return 1;
	}


	while(sigEnd == 0) {
		if(aldl_ftdi_connect(0x0403, 0x6001) != 0) {
			return 1;
		}
		aldl_ftdi_flush();
		if((aldl_send_message(ecm_mode1, 5)) < 0) {
			aldl_ftdi_disconnect();
			return 1;
		}

		nanosleep(&scanTime, NULL);

		aldl_receive_message(ecm_mode1_data, 72);

#ifdef DEBUG
		printf("Received Message: \n");
		printHexString(ecm_mode1_data, 72);
#endif

		//add 7 to which offset you want in data
		int rpm = ecm_mode1_data[41]*25;
		int mph = ecm_mode1_data[38];
		int tps = (int) ecm_mode1_data[49]/2.56;
		int runtime = ecm_mode1_data[46]*256+ecm_mode1_data[47];
		float bpw = (ecm_mode1_data[64]*256+ecm_mode1_data[65])/65.536;
		double mpg = ((((double)mph)/3600))/(((bpw/1000*0.122222)/6.07)*(((double)rpm)/60));
		float volt = ((float)ecm_mode1_data[23])/10;
		int temp = ecm_mode1_data[22]*1.35-40;
		float baro = (ecm_mode1_data[34]+28.06)/2.71;
		printf("Current Engine Runtime: %d secs\n", runtime);
		printf("Current MPG: %.1lf MPG\n", mpg);
		printf("Current Speed: %d MPH\n", mph);
		printf("Current RPM: %d RPM\n", rpm);
		printf("Current Throttle Position: %d%%\n", tps);
		printf("Current Battery Voltage: %.1f Volts\n", volt);
		printf("Current Barometric Pressure: %.2f kpa\n", baro);
		printf("Current Engine Temperature: %d degrees F\n", temp);

		char* malf1[] = {"Error 21: Throttle Position High", "N/A", "N/A", "N/A", "N/A", "Error 15: Coolant Sensor Low Temperature", "Error 14: Coolant Sensor High Temperature", "Error 13: Oxygen Sensor"};
		printErrorCodes(ecm_mode1_data[13], malf1);

		char* malf2[] = {"N/A", "Error 28: Pressure Switch Manifold", "N/A", "N/A", "Error 25: MAT Sensor High", "Error 24: Vehicle Speed Sensor", "Error 23: MAT Sensor Low", "Error 22: Throttle Sensor Low"};
		printErrorCodes(ecm_mode1_data[14], malf2);

		char* malf3[] = {"Error 38: Brake Off", "Error 37: Brake On", "N/A", "Error 35: IAC Failure", "Error 34: MAP Sensor Low", "Error 33: MAP Sensor High", "Error 32: EGR Failure", "Error 31: Governor Failure"};
		printErrorCodes(ecm_mode1_data[15], malf3);

		char* malf4[] = {"N/A", "Error 46: VATS Failure", "Error 45: Oxygen Sensor Rich", "Error 44: Oxygen Sensor Lean", "Error 43: ESC Failure", "Error 42: EST. Monitor", "Error 41: 1X Camshaft Pulse Sensor Failure", "Error 39: TCC Off"};
		printErrorCodes(ecm_mode1_data[16], malf4);

		char* malf5[] = {"N/A", "Error 55: ADU Failure", "Error 54: Fuel Pump Relay Malfunction", "Error 53: System Voltage High", "Error 52: System Voltage Low", "Error 51: PROM Error", "N/A", "N/A"};
		printErrorCodes(ecm_mode1_data[17], malf5);
	}

/*	if((aldl_send_message(ecm_mode1, 4)) != 0) {
		aldl_receive_message(ecm_mode0_data, 4);
#ifdef DEBUG
		printf("Received Message: \n");
		printHexString(ecm_mode0_data, 4);
#endif
	}
*/
	aldl_ftdi_disconnect();
	return 0;
}

#ifdef DEBUG
void printHexString(byte* str, int length) {
	for(int i = 0; i < length; i++) {
		printf("%02X ", str[i]);
	}
	printf("\n");
}
#endif

void printErrorCodes(byte bits, char* errors[]) {
	for(int i=1; i < 9; i++) {
		if(bitValue(bits, i) != 0) {
			printf("%s\n", errors[i-1]);
		}
	}
}

int bitValue(byte num, int bit) {
	if (bit > 0 && bit <= 8)
		return ((num >> (bit-1)) & 1);
	else
		return 0;
}
