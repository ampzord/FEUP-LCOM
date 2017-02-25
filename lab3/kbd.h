#ifndef __KBD_H
#define __KBD_H

int kbd_subscribe_int();

int kbd_unsubscribe_int();

int kbd_handler();

void toggleLeds(unsigned int ledNo);

int LedSwitcher(unsigned int ledNo);

int readInfoFromKBD(unsigned long *data);

int writeInfoToKBD(unsigned long kbd_port);

#endif
