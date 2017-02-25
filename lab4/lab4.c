#include <minix/drivers.h>
#include "test4.h"

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
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <decimal no. - counter.>\"\n"
			"\t service run %s -args \"async <decimal no. - idle_time.>\"\n"
			"\t service run %s -args \"config\"\n",
			"\t service run %s -args \"gesture\"\n", argv[0], argv[0], argv[0],
			argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short counter_packet, idle_time;
	short length;

	//PACKET
	if (strncmp(argv[1], "packet", strlen("packet")) == 0)
	{
		if (argc != 3)
		{
			printf("packet: wrong no. of arguments for test_packet()\n");
			return 1;
		}

		counter_packet = parse_ulong(argv[2], 10);

		if (counter_packet == ULONG_MAX)
			return 1;

		if (counter_packet <= 0)
		{
			printf("packet: wrong no. of arguments for test_packet()\n");
			return 1;
		}

		printf("packet::test_packet(%u)\n", counter_packet);
		return test_packet(counter_packet);
	}
	//ASYNC
	else if (strncmp(argv[1], "async", strlen("async")) == 0)
	{
		if (argc != 3)
		{
			printf("async: wrong no. of arguments for test_async()\n");
			return 1;
		}

		idle_time = parse_ulong(argv[2], 10);

		if (idle_time == ULONG_MAX)
			return 1;

		if (idle_time <= 0)
		{
			printf("async: wrong no. of arguments for test_async()\n");
			return 1;
		}

		printf("async::test_async(%u)\n", idle_time);
		return test_async(idle_time);
	}
	//CONFIG
	else if (strncmp(argv[1], "config", strlen("config")) == 0)
	{
		if (argc != 2)
		{
			printf("config: wrong no. of arguments for test_config()\n");
			return 1;
		}
		printf("config::test_config()\n");
		return test_config();
	}
	//GESTURE
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0)
	{
		if (argc != 3)
		{
			printf("gesture: wrong no. of arguments for test_gesture()\n");
			return 1;
		}

		length = parse_ulong(argv[2], 10);

		if (length == ULONG_MAX)
			return 1;

		printf("gesture::test_gesture(%d)\n", length);
		return test_gesture(length);
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
