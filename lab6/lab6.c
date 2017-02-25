#include <minix/drivers.h>
#include <minix/syslib.h>
#include "rtc.h"

int main(int argc, char **argv)
{
	sef_startup();
	//sys_iopenable(SELF);

	unsigned long seconds;

	int i = 0;
	while (i < 30)
	{
		seconds = readSecondsFromRTC();
		if(seconds < 10)
			printf("0%x\n",seconds);
		else
			printf("%x\n",seconds);
		sleep(1);
		i++;
	}
	return 0;
}
