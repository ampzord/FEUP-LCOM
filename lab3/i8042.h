#ifndef _I8042_H
#define _I8042_H

#define DELAY_US 20000


#define KBD_IRQ 			1
#define KBD_NOTIFICATION 	16



#define KB_ECHO 0xFE
#define KB_RESET 0xFF


#define PAR_ERR	BIT(7)
#define TIMEOUT BIT(6)


/* SCAN */

#define ESC_BREAK 		0x81
#define KBD_TWO_BYTE 	0xE0

/* LEDS */

#define KBD_LED_CMD 	0xED
#define KBD_LED_ERROR 	0xFC
#define ACK				0xFA
#define RESEND 			0xFE
#define ERROR 			0xFC


/* KEYBOARD */

#define KBD_OUT_BUF 	0x60
#define KBD_IN_BUF 		0x64
#define IN_BUF			0x60
#define OUT_BUF			0x60

#endif /* _I8042_H */
