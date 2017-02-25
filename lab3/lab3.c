#include "test3.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <minix/syslib.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();
	sys_enable_iop(SELF);

	if (argc == 1)
	{
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <decimal no. - asm no.>\"\n"
			"\t service run %s -args \"leds <decimal no. - size no.>\"\n"
			"\t service run %s -args \"timedscan <decimal no. - time>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned long time, size, dec_asm;
	unsigned short *leds;

	//SCAN ARGUMENT
	if (strncmp(argv[1], "scan", strlen("scan")) == 0)
	{
		if (argc != 3)
		{
			printf("scan: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		dec_asm = atoi(argv[2]); /* Parses string to unsigned long */

		if (dec_asm > 1 || dec_asm < 0)
		{
			printf("scan: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		printf("scan::kbd_test_scan(%lu)\n", dec_asm);
		return kbd_test_scan(dec_asm);
	}
	//LEDS
	else if (strncmp(argv[1], "leds", strlen("leds")) == 0)
	{
		size = atoi(argv[2]);

		if ((size + 3) != argc)
		{
			printf("leds: wrong no. of arguments for kbd_test_leds(), size and numb. of ele. not match.\n");
			return 1;
		}


		leds = malloc(size * sizeof (unsigned short));

		if (leds == NULL)
		{
			printf("Error while allocating memory for leds array.\n");
			return 1;
		}

		int i = 3;
		int aux = 0;
		unsigned int ledChosen;

		while (aux < size)
		{
			ledChosen = atoi(argv[i]);
			if (ledChosen < 0 || ledChosen > 2)
			{
				printf("leds: Elements of array must be between 0 and 2.\n");
				return 1;
			}
			leds[aux] = (unsigned short) ledChosen; //WORKS!
			i++;
			aux++;
		}

		printf("leds::kbd_test_leds(%lu)\n", size);
		return kbd_test_leds(size,leds);
	}
	//TIMED_SCAN ARGUMENT
	else if (strncmp(argv[1], "timedscan", strlen("timedscan")) == 0)
	{
		if (argc != 3)
		{
			printf(
					"timedscan: wrong no of arguments for kbd_test_timed_scan()\n");
			return 1;
		}
		time = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (time == ULONG_MAX)
			return 1;
		printf("timedscan::kbd_test_timed_scan(%lu)\n", time);
		return kbd_test_timed_scan(time);
	} else
	{
		printf("kbd: %s - no valid function!\n", argv[1]);
		return 1;
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
