#include "mouse.h"
#include "i8254.h"
#include "i8042.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <stdio.h>
#include <stdlib.h>

int mouse_handler()
{
	unsigned long byte;
	int aux;

	aux = sys_inb(OUT_BUF, &byte);

	if (aux != OK)
	{
		printf("Error in sys_inb(OUT_BUF, &byte).\n");
		return 1;
	}

	//First byte of an iteration must have MOUSE_VERIFICATION at 1
	if (mouseCounter == 0 && ((byte & MOUSE_VERIFICATION) == 0) && byte != ACK)
	{
		//printf("\nClearing packets.");
		resetPackets();
		return 1;
	}

	packet[mouseCounter] = byte;
	mouseCounter++;

	//Reset mouseCounter to 0 for next iteration
	if (mouseCounter >= 3)
		mouseCounter = 0;

	//If mouseCounter == 0 we have to print
	if (mouseCounter == 0)
		return 0;
	else
		return 1;
}



int getMouseConfig()
{
	int counter = 0;
	unsigned long data;
	while (counter < 3)
	{
		if (!readInfoThroughKBC(&data))
			packet[counter] = (unsigned short) data;
		else
			return -1;
		counter++;
	}
	return 0;
}

int displayConfig()
{
	printf("\tDISPLAY THE MOUSE CONFIG\n");

	printf("\nMODE\n");
	if (packet[0] & MOUSE_CONFIG_MODE)
		printf(" Remote mode\n");
	else
		printf(" Stream mode\n");

	printf("\nDATA REPORTING\n");
	if (packet[0] & MOUSE_CONFIG_DATA_REPORTING)
		printf(" Data reporting enabled\n");
	else
		printf(" Data reporting disable\n");

	printf("\nSCALING\n");
	if (packet[0] & MOUSE_CONFIG_SCALLING)
		printf(" Scaling: 2:1\n");
	else
		printf(" Scaling: 1:1\n");

	printf("\nLEFT BUTTON\n");
	if (packet[0] & MOUSE_CONFIG_LEFT_BUTTON)
		printf(" Left button: Active\n");
	else
		printf(" Left button: Disabled\n");

	printf("\nMIDDLE BUTTON\n");
	if (packet[0] & MOUSE_CONFIG_MIDDLE_BUTTON)
		printf(" Middle button: Active\n");
	else
		printf(" Middle button: Disabled\n");

	printf("\nRIGHT BUTTON\n");
	if (packet[0] & MOUSE_CONFIG_RIGHT_BUTTON)
		printf(" Right button: Active\n");
	else
		printf(" Right button: Disabled\n");

	printf("\nRESOLUTION\n");
	if ((packet[1] & (BIT(1) | BIT(0))) == 0)
		printf(" 1 count/mm \n");
	else if ((packet[1] & (BIT(1) | BIT(0))) == 1)
		printf(" 2 count/mm \n");
	else if ((packet[1] & (BIT(1) | BIT(0))) == 2)
		printf(" 4 count/mm \n");
	else if ((packet[1] & (BIT(1) | BIT(0))) == 3)
		printf(" 8 count/mm \n");

	printf("\n");
	printf("SAMPLE RATE\n %d\n", packet[2]);
}



int mouse_subscribe_int()
{
	hook_id = MOUSE_NOTIFICATION;

	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)
			!= OK)
	{
		printf("Error while subscribing mouse - Set Policy. \n");
		return 1;
	}
	return 0;
}

int mouse_unsubscribe_int()
{

	if (sys_irqdisable(&hook_id) != OK)
	{
		printf("Error while unsubscribing mouse - Irq Disable\n");
		return 1;
	}

	if (sys_irqrmpolicy(&hook_id) != OK)
	{
		printf("Error while unsubscribing mouse - Remove Policy\n");
		return 1;
	}
	return 0;
}

int writeInfoThroughKBC(unsigned long kbd_port, unsigned long command)
{
	unsigned long stat;
	unsigned int count = 0;
	while (count < 10)
	{
		if (sys_inb(KBD_IN_BUF, &stat) != OK)
		{
			printf("Error no. 1 - (input buff not ready) in function writeInfoToKBD().\n");
			return 1;
		}
		if ((stat & BIT(1)) == 0)
		{
			if (sys_outb(kbd_port, command) != OK)
			{
				printf("Error n  o. 2 - (Can't read from out buff) in function writeInfoToKBD().\n");
				return 2;
			} else
				return 0;	//Everything works right as intended.
		}
		tickdelay(micros_to_ticks(DELAY_US));
		count++;
	}

}

int readInfoThroughKBC(unsigned long *data)
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
			} else
				return 0;	//Everything works right as intended.
		}
		tickdelay(micros_to_ticks(DELAY_US));
		count++;
	}

}

int mouseWrite(unsigned long command)
{
	int count = 0;
	unsigned long data;
	int aux;

	while (count < 10)
	{

		aux = writeInfoThroughKBC(KBD_IN_BUF, MOUSE_WRITE_BYTE);
		if (aux != 0)
		{
			printf(
					"Error on 1st writeInfoThroughKBC(KBD_IN_BUF,MOUSE_WRITE_BYTE).\n");
			return 1;
		}

		aux = readInfoThroughKBC(&data);
		if (aux != 0)
		{
			printf("Error on 1st readInfoThroughKBC(data).\n");
			return 1;
		}

		aux = writeInfoThroughKBC(KBD_OUT_BUF, command);
		if (aux != 0)
		{
			printf("Error on 2nd writeInfoThroughKBC(KBD_IN_BUF,command).\n");
			return 1;
		}

		aux = readInfoThroughKBC(&data);
		if (aux != 0)
		{
			printf("Error on 2nd readInfoThroughKBC(data).\n");
			return 1;
		}
		if (data == ACK)
			return 0;
		count++;

	}

}

void printPackets()
{
	printf("\n");
	printf("B1=0x%X ", packet[0]);
	printf("B2=0x%X ", packet[1]);
	printf("B3=0x%X ", packet[2]);

	//MOUSE_LEFT_BUTTON
	if (packet[0] & MOUSE_LEFT_BUTTON)
		printf("LB=1 ");
	else
		printf("LB=0 ");

	//MOUSE_MIDDLE_BUTTON
	if (packet[0] & MOUSE_MIDDLE_BUTTON)
		printf("MB=1 ");
	else
		printf("MB=0 ");

	//MOUSE_RIGHT_BUTTON
	if (packet[0] & MOUSE_RIGHT_BUTTON)
		printf("RB=1 ");
	else
		printf("RB=0 ");

	//MOUSE_X_SIGN
	if (packet[0] & MOUSE_X_SIGN) {
		//printf("X_SIGN=1 ");
		xSign = 1;
	}
	else {
		//printf("X_SIGN=0 ");
		xSign = 0;
	}

	//MOUSE_Y_SIGN
	if (packet[0] & MOUSE_Y_SIGN) {
		//printf("Y_SIGN=1 ");
		ySign = 1;
	}
	else {
		//printf("Y_SIGN=0 ");
		ySign = 0;
	}

	//MOUSE_X_OVERFLOW
	if (packet[0] & MOUSE_X_OVERFLOW) {
		printf("XOV=1 ");
		hasOverflowX= 1;
	}
	else {
		printf("XOV=0 ");
		hasOverflowX = 0;
	}

	//MOUSE_Y_OVERFLOW
	if (packet[0] & MOUSE_Y_OVERFLOW) {
		printf("YOV=1 ");
		hasOverflowY = 1;
	}
	else {
		printf("YOV=0 ");
		hasOverflowY = 0;
	}

	printf("X=%d ", (char) packet[1]);
	printf("Y=%d ", (char) packet[2]);
	printf("\n");
}

void resetPackets()
{
	packet[0] = 0;
	packet[1] = 0;
	packet[2] = 0;
}

int isRightButtonPressed()
{
	if (packet[0] & MOUSE_RIGHT_BUTTON)
		return 1;
	else
		return 0;
}

int isPositiveSlope(double slope) {
	if (slope <= MAX_SLOPE && slope >= MIN_SLOPE)
		return 1;
	return 0;
}

void resetXYVars()
{
	posX = 0;
	posY = 0;
	distanceY = 0;
}

int handler(short length) {

	posX = (char) packet[1];
	posY = (char) packet[2];

	evt event;

	if (!isRightButtonPressed()) {
		resetXYVars();
		event.mode = RUP;
		if (check_positive_line(&event, length))
			return 1;
	}
	//Right Click is Pressed
	else {
		event.mode = RDOWN;
		if (check_positive_line(&event, length))
			return 1;
	}
	//Means we are moving
	if (posX != 0 || posY != 0) {
		event.mode = MOVE;
		if (check_positive_line(&event, length))
			return 1;
	}

	return 0;
}


int	check_positive_line(evt *evt, int length) {
	static state_t st = INIT; // initial state; keep state
	switch (st) {
	case INIT:
		if( evt->mode == RDOWN )
			st = DRAW;
		break;
	case DRAW:
		if( evt->mode == MOVE ) {
			if (check_positive_slope(length)) {
				st = COMPLETE;
				break;
			}
		} else if( evt->mode == RUP ) {
			st = INIT;
			resetXYVars();
			break;
		}
	default:
		break;
	}

	if (st == COMPLETE)
		return 1;
	return 0;
}

int check_positive_slope(short length) {
	double slope = 0;

	if (length > 0) {
		//increasing x and y
		if (posX > 0 && posY > 0 && xSign == 0 && ySign == 0)
		{
			distanceY += posY;
			if (distanceY > length)
			{
				slope = posY / posX;
				if (isPositiveSlope(slope))
					return 1;
			}
		}
		else {
			distanceY = 0;
		}
	}
	//length < 0
	else {
		//decreasing x and y
		if (posX < 0 && posY < 0 && xSign == 1 && ySign == 1)
		{
			distanceY += abs(posY);
			if (distanceY > abs(length)) {
				slope = posY / posX;
				if (isPositiveSlope(slope))
					return 1;
			}
		}
		else {
			distanceY = 0;
		}
	}

	return 0;
}
