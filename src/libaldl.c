#include "../inc/libaldl.h"

#include <stdio.h>

#include <ftdi.h>

struct ftdi_context *ftdi;

int aldl_ftdi_connect(int vendor, int device) {
	int ret;

	if((ftdi = ftdi_new()) == 0) {
		fprintf(stderr, "ftdi_new failed\n");
		return -1;
	}

	if((ret = ftdi_usb_open(ftdi, vendor, device)) < 0) {
		fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		ftdi_free(ftdi);
		return -1;
	}

	ftdi_set_baudrate(ftdi, 8192);
	ftdi_set_latency_timer(ftdi, 2);

	return 0;
}

int aldl_send_message(byte* command, int length) {
	ftdi_usb_purge_buffers(ftdi);
	ftdi_write_data(ftdi,(unsigned char *)command,bytes);
}

int aldl_receive_message(byte* buffer, int length) {
	int response = 0;
	response = ftdi_read_data(ftdi,(unsigned char *)buffer,length);
	return response;
}
