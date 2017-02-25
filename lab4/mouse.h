#ifndef __MOUSE_H
#define __MOUSE_H

static int distanceY = 0;

static int hook_id;

static int mouseCounter = 0;

static int xSign;

static int ySign;

static int posX, posY;

static unsigned short packet[] = { 0, 0, 0 };

static int hasOverflowX = 0;

static int hasOverflowY = 0;

typedef enum {INIT, DRAW, COMPLETE} state_t;

typedef enum {RDOWN, RUP, MOVE} ev_type_t;

typedef struct{
        ev_type_t mode;
}evt;


int handler(short length);

int mouse_handler();

int mouse_subscribe_int();

int mouse_unsubscribe_int();

int writeInfoThroughKBC(unsigned long kbd_port, unsigned long command);

int readInfoThroughKBC(unsigned long *data);

int mouseWrite(unsigned long command);

void printPackets();

void resetPackets();

int displayConfig();

void resetXYVars();

void setRightButtonPressed();

int isRightButtonPressed();

int slope_handler(short length);

int	check_positive_line(evt *evt, int length);

int check_positive_slope(short length);

#endif /* __MOUSE_H */
