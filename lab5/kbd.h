#ifndef __KBD_H
#define __KBD_H

#include "i8254.h"
#include "i8042.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

int kbd_subscribe_int();

int kbd_unsubscribe_int();

int kbd_handler();

void toggleLeds(unsigned int ledNo);

int LedSwitcher(unsigned int ledNo);

int readInfoFromKBD(unsigned long *data);

int writeInfoToKBD(unsigned long kbd_port);

static int hook_id;
static int twoBytes = 0;
static unsigned char NUM_LOCK = 0;
static unsigned char CAPS_LOCK = 0;
static unsigned char SCROLL_LOCK = 0;



#endif
