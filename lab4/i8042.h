#ifndef _I8042_H
#define _I8042_H

#define DELAY_US 20000

#define KBD_IRQ 				1
#define KBD_NOTIFICATION 		16

#define KB_ECHO 				0xFE
#define KB_RESET 				0xFF

#define PAR_ERR					BIT(7)
#define TIMEOUT 				BIT(6)

/* SCAN */

#define ESC_BREAK 				0x81
#define KBD_TWO_BYTE 			0xE0

/* LEDS */

#define KBD_LED_CMD 			0xED
#define KBD_LED_ERROR	 		0xFC
#define ACK						0xFA
#define RESEND 					0xFE
#define ERROR 					0xFC

/* KEYBOARD */

#define KBD_OUT_BUF 			0x60
#define KBD_IN_BUF 				0x64
#define IN_BUF					0x60
#define OUT_BUF					0x60

/* MOUSE */

#define MOUSE_NOTIFICATION 		17
#define MOUSE_IRQ				12
#define MOUSE_WRITE_BYTE 		0xD4
#define MOUSE_STAT_REQUEST		0xE9
#define SET_STREAM_MODE 		0xEA
#define ENABLE_STREAM_MODE		0xF4
#define DISABLE_STREAM_MODE 	0xF5

/* MOUSE BYTE */

#define MOUSE_LEFT_BUTTON 		BIT(0)
#define MOUSE_RIGHT_BUTTON 		BIT(1)
#define MOUSE_MIDDLE_BUTTON 	BIT(2)
#define MOUSE_VERIFICATION 		BIT(3)
#define MOUSE_X_SIGN 			BIT(4)
#define MOUSE_Y_SIGN 			BIT(5)
#define MOUSE_X_OVERFLOW 		BIT(6)
#define MOUSE_Y_OVERFLOW 		BIT(7)

/* MOUSE CONFIG */

#define MOUSE_CONFIG_LEFT_BUTTON	BIT(2)
#define MOUSE_CONFIG_RIGHT_BUTTON	BIT(0)
#define MOUSE_CONFIG_MIDDLE_BUTTON	BIT(1)
#define MOUSE_CONFIG_SCALLING		BIT(4)
#define MOUSE_CONFIG_DATA_REPORTING	BIT(5)
#define MOUSE_CONFIG_MODE			BIT(6)



/* MOUSE SLOPE */

#define MAX_SLOPE				1.6
#define MIN_SLOPE				0.75


#define TRUE 1
#define FALSE 0



#endif /* _I8042_H */
