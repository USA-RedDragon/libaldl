#ifndef _LIBALDL_H_
#define _LIBALDL_H_

typedef unsigned char byte;

extern int aldl_ftdi_connect(int vendor, int device);
extern int aldl_send_message(byte* command, int length);
extern int aldl_receive_message(byte* buffer, int length);
extern void aldl_ftdi_disconnect();
extern void aldl_ftdi_flush();

#endif
