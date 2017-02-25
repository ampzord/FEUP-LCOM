#include <minix/drivers.h>
#include "test5.h"
#include "pixmap.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();

	if (argc == 1)
	{
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf(
			"Usage: one of the following:\n"
			"\t service run %s -args \"init <hex-mode> <decimal-delay> \" \n"
			"\t service run %s -args \"square <decimal-x> <decimal-y> <decimal-size> <hex-color>\" \n"
			"\t service run %s -args \"line <decimal-xi> <decimal-yi> <decimal-xf> <<decimal-yf> <hex-color>\" \n"
			"\t service run %s -args \"xpm <decimal-xi> <decimal-yi> <xpm-pointer>\" \n"
			"\t service run %s -args \"move <decimal-xi> <decimal-yi> <xpm-pointer> <deicmal-orientation> <decimal-delta> <decimal-time>\" \n"
			"\t service run %s -args \"controller \" \n", argv[0],
			argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short mode, delay, x, y, size, xi, yi, xf, yf, hor, time;
	short delta;
	unsigned long color;
	char ** xpm;

	//INIT
	if (strncmp(argv[1], "init", strlen("init")) == 0)
	{
		if (argc != 4)
		{
			printf("init: wrong no. of arguments for test_init()\n");
			return 1;
		}

		mode = parse_ulong(argv[2], 16);
		if (mode == ULONG_MAX)
			return 1;

		delay = parse_ulong(argv[3], 10);
		if (delay == ULONG_MAX)
			return 1;

		printf("LAB5::test_init(%x,%d)\n", mode, delay);
		(char*) test_init(mode, delay);
		return 1;
	}
	//SQUARE
	else if (strncmp(argv[1], "square", strlen("square")) == 0)
	{
		if (argc != 6)
		{
			printf("square: wrong no. of arguments for test_square()\n");
			return 1;
		}

		x = parse_ulong(argv[2], 10);
		if (x == ULONG_MAX)
			return 1;

		y = parse_ulong(argv[3], 10);
		if (y == ULONG_MAX)
			return 1;

		size = parse_ulong(argv[4], 10);
		if (size == ULONG_MAX)
			return 1;

		color = parse_ulong(argv[5], 10);
		if (size == ULONG_MAX)
			return 1;

		printf("square::test_square(%d,%d,%d,%x)\n", x, y, size, color);
		return test_square(x, y, size, color);
	}
	//LINE
	else if (strncmp(argv[1], "line", strlen("line")) == 0)
	{
		if (argc != 7)
		{
			printf("line: wrong no. of arguments for test_line()\n");
			return 1;
		}

		xi = parse_ulong(argv[2], 10);
		if (x == ULONG_MAX)
			return 1;

		yi = parse_ulong(argv[3], 10);
		if (y == ULONG_MAX)
			return 1;

		xf = parse_ulong(argv[4], 10);
		if (x == ULONG_MAX)
			return 1;

		yf = parse_ulong(argv[5], 10);
		if (y == ULONG_MAX)
			return 1;

		color = parse_ulong(argv[6], 10);
		if (size == ULONG_MAX)
			return 1;

		printf("line::test_line(%d,%d,%d,%d,%d)\n", xi, yi, xf, yf, color);
		return test_line(xi, yi, xf, yf, color);
	}
	//XPM
	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0)
	{
		if (argc != 5)
		{
			printf("xpm: wrong no. of arguments for test_xpm()\n");
			return 1;
		}

		xi = parse_ulong(argv[2], 10);
		if (x == ULONG_MAX)
			return 1;

		yi = parse_ulong(argv[3], 10);
		if (y == ULONG_MAX)
			return 1;

		if (strncmp(argv[4], "pic1", strlen("pic1")) == 0)
		{
			xpm = pic1;
		}

		if (strncmp(argv[4], "pic2", strlen("pic2")) == 0)
		{
			xpm = pic2;
		}

		if (strncmp(argv[4], "cross", strlen("cross")) == 0)
		{
			xpm = cross;
		}

		if (strncmp(argv[4], "pic3", strlen("pic3")) == 0)
		{
			xpm = pic3;
		}

		if (strncmp(argv[4], "penguin", strlen("penguin")) == 0)
		{
			xpm = penguin;
		}

		printf("xpm::test_xpm(%d,%d,%s)\n", xi, yi, xpm);
		return test_xpm(xi, yi, xpm);
	}
	//MOVE
	else if (strncmp(argv[1], "move", strlen("move")) == 0)
	{
		if (argc != 8)
		{
			printf("move: wrong no. of arguments for test_move()\n");
			return 1;
		}

		xi = parse_ulong(argv[2], 10);
		if (x == ULONG_MAX)
			return 1;

		yi = parse_ulong(argv[3], 10);
		if (y == ULONG_MAX)
			return 1;

		if (strncmp(argv[4], "pic1", strlen("pic1")) == 0)
		{
			xpm = pic1;
		}

		if (strncmp(argv[4], "pic2", strlen("pic2")) == 0)
		{
			xpm = pic2;
		}

		if (strncmp(argv[4], "cross", strlen("cross")) == 0)
		{
			xpm = cross;
		}

		if (strncmp(argv[4], "pic3", strlen("pic3")) == 0)
		{
			xpm = pic3;
		}

		if (strncmp(argv[4], "penguin", strlen("penguin")) == 0)
		{
			xpm = penguin;
		}

		hor = parse_ulong(argv[5], 10);
		if (hor == ULONG_MAX)
			return 1;

		delta = parse_ulong(argv[6], 10);
		if (delta == ULONG_MAX)
			return 1;

		time = parse_ulong(argv[7], 10);
		if (time == ULONG_MAX)
			return 1;

		printf("move::test_move(%d,%d,%s,%d,%d,%d)\n", xi, yi, xpm, hor, delta,
				time);
		return test_move(xi, yi, xpm, hor, delta, time);
	}
	//CONTROLLER
	else if (strncmp(argv[1], "controller", strlen("controller")) == 0)
	{
		if (argc != 2)
		{
			printf(
					"controller: wrong no. of arguments for test_controller()\n");
			return 1;
		}

		printf("controller::test_controller()\n");
		return test_controller();
	}
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0))
	{
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str)
	{
		printf("kbd: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
