#include "test5.h"
#include "video_gr.h"
#include "kbd.h"
#include "i8042.h"
#include "i8254.h"
#include "sprite.h"
#include "timer.h"
#include "vbe.h"

void *test_init(unsigned short mode, unsigned short delay)
{

	vg_init(mode);
	sleep(delay);
	vg_exit();
	vbe_mode_info_t vram;
	//1 means error
	if ((vbe_get_mode_info(mode, &vram)) == 1)
	{
		return;
	}
	printf("\n\n\nVRAM Physical Address : 0x%X", vram.PhysBasePtr);
}

int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color)
{
	int ipc_status, r;
	hook_id = KBD_NOTIFICATION;
	int irq_kbd_set = (1 << hook_id);
	message msg;
	unsigned long scancode;

	char *video_mem = vg_init(VIDEO_GR_MODE_105);

	if (x + size > getHRES() || y + size > getVRES())
	{
		vg_exit();
		printf("Out of range.\n");
		return 1;
	}

	int height, width;

	for (height = 0; height < size; height++)
	{
		for (width = 0; width < size; width++)
		{
			colorPixel(x + width, y + height, video_mem, color);
		}
	}

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
				if (msg.NOTIFY_ARG & irq_kbd_set)
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

	kbd_unsubscribe_int();

	vg_exit();
	return 0;

}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color)
{
	int ipc_status, r;
	hook_id = KBD_NOTIFICATION;
	int irq_kbd_set = (1 << hook_id);
	message msg;
	unsigned long scancode;

	char *video_mem = vg_init(VIDEO_GR_MODE_105);

	if (xi > getHRES() || yi > getVRES() || xf > getHRES() || yf > getVRES())
	{
		vg_exit();
		printf("Out of range.\n");
		return 1;
	}

	drawLine(xi, yi, xf, yf, video_mem, color);

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
				if (msg.NOTIFY_ARG & irq_kbd_set)
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

	kbd_unsubscribe_int();
	vg_exit();
	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[])
{
	int ipc_status, r;
	hook_id = KBD_NOTIFICATION;
	int irq_kbd_set = (1 << hook_id);
	message msg;
	unsigned long scancode;

	Sprite *s = create_sprite(xi, yi, xpm);

	if ((xi + s->width) > getHRES() || (yi + s->height) > getVRES())
	{
		printf("Out of range.\n");
		return 1;
	}

	if (vg_init(VIDEO_GR_MODE_105) == NULL)
	{
		printf("Error on vg_init() from test_xpm()\n");
		return 1;
	}

	drawSprite(s);

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
				if (msg.NOTIFY_ARG & irq_kbd_set)
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

	kbd_unsubscribe_int();

	vg_exit();
	return 0;
}

int test_move(unsigned short xi, unsigned short yi, char *xpm[], unsigned short hor, short delta, unsigned short time)
{
	int ipc_status, r;
	message msg;

	//TIMER
	int cpu_time = time * DEFAULT_FREQUENCY; //assuming timer is running on default frequency-60 hz
	hook_id = NOTIFICATION_ID;
	int irq_timer_set = (1 << hook_id);

	timer_subscribe_int();
	timer_set_square(0, 60);

	//KEYBOARD
	hook_id = KBD_NOTIFICATION;
	int irq_kbd_set = (1 << hook_id);
	unsigned long scancode;

	kbd_subscribe_int();

	Sprite *s = create_sprite(xi, yi, xpm);

	if ((xi + s->width) > getHRES() || (yi + s->height) > getVRES())
	{
		printf("Out of range.\n");
		return 1;
	}

	char *video_mem = vg_init(VIDEO_GR_MODE_105);
	//	if (vg_init(VIDEO_GR_MODE_105) == NULL)
	//	{
	//		printf("Error on vg_init() from test_move()\n");
	//		return 1;
	//	}

	clearScreen();

	//speed = distance / time , speed is per second need to divide later by 60 for speed by frame
	double speed = (double) delta / time;
	int beyondLimit = 0;

	drawSprite(s);

	while (scancode != -1) //scancode not ESC_BREAK_CODE
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
				//TIMER
				if (msg.NOTIFY_ARG & irq_timer_set)
				{
					if (counter < cpu_time)
					{
						//clearScreen();

						//clear sprite from the screen
						int height, width;
						for (height = 0; height < s->height; height++)
						{
							for (width = 0; width < s->width; width++)
							{
								colorPixel(width + s->x, height + s->y, video_mem, 0);
							}
						}

						if (beyondLimit == 0)
						{
							if (hor != 0)
							{
								s->xspeed = speed;
								s->yspeed = 0;
								s->x += (s->xspeed / DEFAULT_FREQUENCY); //speed per frame
							}
							//VERTICAL
							else
							{
								s->xspeed = 0;
								s->yspeed = speed;
								s->y += (s->yspeed / DEFAULT_FREQUENCY); //speed per frame
							}
						}

						//check limits
						// (s->x < 0) || (s->y < 0)
						if ((s->x + s->width) > getHRES() || (s->y + s->height) > getVRES())
						{
							beyondLimit = 1;
						}
						drawSprite(s);
						counter++;

					}
				}
				//KEYBOARD
				if (msg.NOTIFY_ARG & irq_kbd_set)
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

	kbd_unsubscribe_int();

	timer_unsubscribe_int();

	vg_exit();

	if (beyondLimit)
	{
		printf("Sprite reached the limit of the screen.");
	}

	return 0;
}

int test_controller()
{
	uint16_t *ptr;
	vbe_information_block_t info_ctrl;

	ptr = vbe_get_information(&info_ctrl);

	if (ptr == NULL)
	{
		printf("Error while acessing information\n.");
		return 1;
	}

	printf("\n\n");

	printf("\t\tVBE CONTROLLER INFORMATION\n\n");

	printf("\tThe controller capabilities :\n\n");

	//DAC
	if (info_ctrl.capabilities[0] & DAC)
	{
		printf("DAC width is switchable to 8 bits per primary color.\n");
	} else
	{
		printf("DAC is fixed width, with 6 bits per primary color.\n");
	}

	//VGA
	if (info_ctrl.capabilities[0] & VGA_COMPA)
	{
		printf("Controller is not VGA compatible.\n");
	} else
	{
		printf("Controller is VGA compatible.\n");
	}

	//RAMDAC
	if (info_ctrl.capabilities[0] & RAMDAC)
	{
		printf("When programming large block of information to the RAMDAC, use the blank bit in Function 09h.\n");
	} else
	{
		printf("Normal RAMDAC operation.\n");
	}

	//D3-31 = Reserved

	//Size of VRAM Memory
	printf("Size of VRAM Memory in blocks of 64kb : %d\n", info_ctrl.total_memory);

	//List of mode numbers (in hexadecimal) supported
	printf("List of mode numbers (in hexadecimal) supported :\n");
	int counter = 1;
	while (*ptr != 0x0FFFF)
	{
		printf("%d : 0x%X\t", counter, (unsigned short) *ptr);
		ptr++;
		counter++;
	}
	printf("\n");

	return 0;

}

