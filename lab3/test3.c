#include "test3.h"
#include "i8042.h"
#include "i8254.h"
#include "timer.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "ASMkbd.S"

int kbd_test_scan(unsigned short ass)
{
	int ipc_status, r;
	hook_id = KBD_NOTIFICATION;
	int irq_set = (1 << hook_id);
	message msg;
	unsigned long scancode;

	kbd_subscribe_int();

	while (scancode != -1)
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
			{
				if (msg.NOTIFY_ARG & irq_set)
				{
					if (ass == 0)
						scancode = kbd_handler();
					/*else
						scancode = ASMkbd();*/
				}
				break;
			}
			default:
				break;
			}
		}
	}
	if (scancode == -1)
		printf("Escape Key pressed\n");

	kbd_unsubscribe_int();
	return 0;
}

int kbd_test_leds(unsigned short n, unsigned short *leds)
{
	hook_id = NOTIFICATION_ID;
	int irq_set = (1 << hook_id);
	int ipc_status, r, cpu_time = (n + 1) * DEFAULT_FREQUENCY;
	message msg;
	int i = 0, counter = 0; //variable to count the number of interruptions
	int aux = 0; //aux variable to help with output

	timer_subscribe_int();

	while (counter < cpu_time)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d\n", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set)
				{
					counter++;
					if (counter % DEFAULT_FREQUENCY == 0)
					{
						if (i == 0)
						{
							ledSwitcher(-1);
						} else
						{
							ledSwitcher(leds[aux]);
							aux++;
						}
						i++;

					}
				}
				break;
			default:
				break;
			}
		} else
		{
			//NOTHING HAPPENS
		}
	}

	timer_unsubscribe_int();
	return 0;
}

int kbd_test_timed_scan(unsigned short n)
{
	int ipc_status, r;
	hook_id = KBD_NOTIFICATION;
	int irq_set = (1 << hook_id);
	message msg;
	unsigned long scancode;


	timer_subscribe_int();
	timer_set_square(0, 60);

	kbd_subscribe_int();
	int counter = 0;

	while (scancode != -1 && counter < (n * DEFAULT_FREQUENCY))
	{
		counter++;
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
			{
				if (msg.NOTIFY_ARG & irq_set)
				{
					scancode = kbd_handler();
				}
				break;
			}
			default:
				break;
			}
		}
	}

	if (scancode == -1)
		printf("Escape Key pressed\n");
	else
		printf("Time is up! %u seconds passed\n", n);

	kbd_unsubscribe_int();
	timer_unsubscribe_int();


	return 0;
}
