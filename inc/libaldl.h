#ifndef _LIBALDL_H_
#define _LIBALDL_H_

extern int aldl_ftdi_connect(int vendor, int device);
extern int aldl_send_message(byte* command, int length);
extern int aldl_receive_message(byte* buffer, int length);

#endif
