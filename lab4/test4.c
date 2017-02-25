#include "test4.h"
#include "i8042.h"
#include "i8254.h"
#include "timer.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "mouse.h"

int test_packet(unsigned short cnt)
{
	int ipc_status, r;
	message msg;
	int count = 0;
	int aux;
	int irq_set = BIT(MOUSE_NOTIFICATION);

	mouse_subscribe_int();

	aux = mouseWrite(SET_STREAM_MODE);

	if (aux != 0)
	{
		printf("Error setting stream mode ON. \n");
		return 1;
	}

	aux = mouseWrite(ENABLE_STREAM_MODE);

	if (aux != 0)
	{
		printf("Error setting data reporting ON. \n");
		return 1;
	}

	resetPackets();

	printf("\n       PRINTING PACKETS IN A USER FRIENDLY WAY: \n");

	while (count < cnt)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set)
				{
					aux = mouse_handler();
					if (aux == 0)
					{
						count++;
						printf("\n                     Packet %d:          \n ",
								count);
						printPackets();
					}
				}

				break;
			default:
				break;
			}
		}
	}

	aux = mouseWrite(DISABLE_STREAM_MODE);

	if (aux != 0)
	{
		printf("Error while disabling stream mode. \n");
	}

	mouse_unsubscribe_int();
	return 0;
}

int test_async(unsigned short idle_time)
{
	int ipc_status, r;
	message msg;
	int count = 0, counter = 0;
	int aux;
	int irq_set = BIT(MOUSE_NOTIFICATION);

	timer_subscribe_int();
	mouse_subscribe_int();

	aux = mouseWrite(SET_STREAM_MODE);

	if (aux != 0)
	{
		printf("Error setting stream mode ON. \n");
		return 1;
	}

	aux = mouseWrite(ENABLE_STREAM_MODE);

	if (aux != 0)
	{
		printf("Error setting data reporting ON. \n");
		return 1;
	}

	resetPackets();

	printf("\n       PRINTING PACKETS IN A USER FRIENDLY WAY: \n");

	while (counter < (idle_time * DEFAULT_FREQUENCY))
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set)
				{
					aux = mouse_handler();
					if (aux == 0)
					{
						counter = 0;
						printf("\n                     Packet %d:          \n ",
								count);
						count++;
						printPackets();
					}
				}
				counter++;
				break;
			default:
				break;
			}
		}
	}

	aux = mouseWrite(DISABLE_STREAM_MODE);

	if (aux != 0)
	{
		printf("Error while disabling stream mode. \n");
	}

	mouse_unsubscribe_int();
	timer_unsubscribe_int();
	printf("\nTimeout! %u seconds passed without receiving packets\n",
			idle_time);
	return 0;
}

int test_config()
{
	mouse_subscribe_int();

	int aux = mouseWrite(DISABLE_STREAM_MODE);
	if (aux != 0)
	{
		printf("Error while disabling stream mode. \n");
	}

	mouseWrite(MOUSE_STAT_REQUEST);
	getMouseConfig();

	displayConfig();

	mouse_unsubscribe_int();
	return 0;
}

int test_gesture(short length)
{
	int ipc_status, r;
	message msg;
	int count = 0;
	int aux;
	int irq_set = BIT(MOUSE_NOTIFICATION);
	int positiveSlope = FALSE;

	timer_subscribe_int();
	mouse_subscribe_int();

	aux = mouseWrite(SET_STREAM_MODE);

	if (aux != 0)
	{
		printf("Error setting stream mode ON. \n");
		return 1;
	}

	aux = mouseWrite(ENABLE_STREAM_MODE);

	if (aux != 0)
	{
		printf("Error setting data reporting ON. \n");
		return 1;
	}

	resetPackets();

	printf("\n       PRINTING PACKETS IN A USER FRIENDLY WAY: \n");

	while (!positiveSlope)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set)
				{
					aux = mouse_handler();
					if (aux == 0)
					{
						printf("\n                     Packet %d:          \n ", count);
						count++;

						printPackets();

						if (hasOverflowX == 0 || hasOverflowY == 0)
							positiveSlope = handler(length);
					}
				}
				break;
			default:
				break;
			}
		}
	}

	aux = mouseWrite(DISABLE_STREAM_MODE);

	if (aux != 0)
	{
		printf("Error while disabling stream mode. \n");
	}

	mouse_unsubscribe_int();
	timer_unsubscribe_int();

	if (length > 0)
		printf("\n\nDetected positive slope with RB clicked. Leaving function\n");
	else
		printf("\n\nDetected negative slope with RB clicked. Leaving function\n");
	return 0;

}
