#include "kbd.h"


int kbd_subscribe_int()
{
	hook_id = KBD_NOTIFICATION;
	if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK)
	{
		printf("Error while subscribing keyboard - Set Policy\n");
		return 1;
	}
	if (sys_irqenable(&hook_id) != OK)
	{
		printf("Error while subscribing keyboard - Irq Enable\n");
		return 1;
	}
	return 0;
}

int kbd_unsubscribe_int()
{
	if (sys_irqdisable(&hook_id) != OK)
	{
		printf("Error while unsubscribing keyboard - Irq Disable\n");
		return 1;
	}

	if (sys_irqrmpolicy(&hook_id) != OK)
	{
		printf("Error while unsubscribing keyboard - Remove Policy\n");
		return 1;
	}
	return 0;
}

int kbd_handler()
{
	unsigned long scancode;

	//Check if info read was done correctly
	if (readInfoFromKBD(&scancode) != 0)
	{
		return 1;
	}

	// 2 bytes
	if (scancode == KBD_TWO_BYTE)
	{
		twoBytes = 1;
		return 0;	//return function so it reads next byte in next iteration
	}

	if (twoBytes)
	{
		scancode = scancode | (KBD_TWO_BYTE << 8);
		twoBytes = 0;
	}

	//BREAK CODE
	if (scancode & BIT(7))
	{
		printf("Break Code : 0x%X \n\n", scancode);
		if (scancode == ESC_BREAK)
			return -1;
	}
	//MAKE CODE
	else
	{
		printf("Make Code : 0x%X \n", scancode);
	}
	return 0;
}

int readInfoFromKBD(unsigned long *data)
{
	unsigned long stat;
	unsigned int count = 0;

	while (count < 10)
	{
		if (sys_inb(KBD_IN_BUF, &stat) != OK)
		{
			printf(
					"Error no. 1 (check input buffer) - in function readInfoFromKBD().\n");
			return 1;
		}

		if (stat & BIT(0))
		{
			if (sys_inb(KBD_OUT_BUF, data) != OK)
			{
				printf(
						"Error no. 2 (writing from out buff to data) in function readInfoFromKBD().\n");
				return 2;
			}

			if ((stat & (PAR_ERR | TIMEOUT)) != 0)
			{
				printf(
						"Error no. 3 (invalid data error) - in function readInfoFromKBD().\n");
				return 3;
			}
			//Everything works right as intended.
			else
				return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		count++;
	}
}

int writeInfoToKBD(unsigned long kbd_port)
{
	unsigned long stat;
	unsigned int count = 0;

	while (count < 10)
	{
		if (sys_inb(KBD_IN_BUF, &stat) != OK)
		{
			printf(
					"Error no. 1 - (input buff not ready) in function writeInfoToKBD().\n");
			return 1;
		}

		if ((stat & BIT(1)) == 0)
		{
			if (sys_outb(KBD_OUT_BUF, kbd_port) != OK)
			{
				printf(
						"Error no. 2 - (Can't read from out buff) in function writeInfoToKBD().\n");
				return 2;
			}
			//Everything works right as intended.
			else
				return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		count++;
	}
}

void toggleLeds(unsigned int ledNo)
{
	switch (ledNo)
	{
	case 0:
		if (NUM_LOCK == 0)
		{
			NUM_LOCK = BIT(1);
			printf("Turning NUM_LOCK ON. \n");
		} else
		{
			NUM_LOCK = 0;
			printf("Turning NUM_LOCK OFF. \n");
		}
		break;
	case 1:
		if (CAPS_LOCK == 0)
		{
			CAPS_LOCK = BIT(2);
			printf("Turning CAPS_LOCK ON. \n");
		} else
		{
			CAPS_LOCK = 0;
			printf("Turning CAPS_LOCK OFF. \n");
		}
		break;
	case 2:
		if (SCROLL_LOCK == 0)
		{
			SCROLL_LOCK = BIT(0);
			printf("Turning SCROLL_LOCK ON. \n");
		} else
		{
			SCROLL_LOCK = 0;
			printf("Turning SCROLL_LOCK OFF. \n");
		}
		break;
	default:
		NUM_LOCK = 0;
		SCROLL_LOCK = 0;
		CAPS_LOCK = 0;
		printf("Turning every led off. \n");
	}
	return;
}

int ledSwitcher(unsigned int ledNo)
{
	kbd_subscribe_int();

	unsigned long data = 0;
	unsigned int counter = 0;
	unsigned short ledStatus = 0;

	toggleLeds(ledNo);
	ledStatus = SCROLL_LOCK + NUM_LOCK + CAPS_LOCK;

	//WRITE IN KBD_LED_CMD PORT
	writeInfoToKBD(KBD_LED_CMD);
	readInfoFromKBD(&data);

	while (data == RESEND || data == ERROR)
	{
		readInfoFromKBD(&data);
		//check for infinite read
		if (counter > 10)
		{
			break;
		}
		counter++;
	}

	if (counter == 11)
	{
		return 1;
	}

	counter = 0; //Re-initialize our counter

	// WRITE LED STATUS IN PORT
	writeInfoToKBD(ledStatus);
	readInfoFromKBD(&data);

	while (data == RESEND || data == ERROR)
	{
		readInfoFromKBD(&data);
		//check for infinite read
		if (counter > 10)
		{
			break;
		}
		counter++;
	}

	if (counter == 11)
		return 1;

	kbd_unsubscribe_int();

	return 0;
}
