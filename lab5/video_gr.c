#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "i8042.h"
#include "sprite.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xE0000000
#define H_RES           1024
#define V_RES		  	768
#define BITS_PER_PIXEL	8

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */

static unsigned h_res = 1024;
static unsigned v_res = 768;
static unsigned bits_per_pixel = 8;

void *vg_init(unsigned short mode)
{
	unsigned video_mem_size;
	vbe_mode_info_t data;

	struct reg86u reg86;
	struct mem_range mr;

	reg86.u.b.intno = VBE_INT10;
	reg86.u.w.ax = VBE_AX;
	reg86.u.w.bx = 1 << 14 | mode;

	if (sys_int86(&reg86) != OK)
	{
		printf("set_vbe_mode: sys_int86() failed \n");
		return;
	}

	int aux = vbe_get_mode_info(mode, &data);
	if (aux == 1)
	{
		printf("Error on get_mode_info()\n");
		return NULL;
	}

	h_res = data.XResolution;
	v_res = data.YResolution;
	bits_per_pixel = data.BitsPerPixel;

	video_mem_size = h_res * v_res * bits_per_pixel;

	mr.mr_base = data.PhysBasePtr;
	mr.mr_limit = mr.mr_base + video_mem_size;

	if (OK != (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
	{
		panic("sys_privctl (ADD_MEM) failed: %d\n", reg86);
	}

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, video_mem_size);

	if (video_mem == MAP_FAILED)
		panic("Couldn't map video memory\n");

	return video_mem;
}

unsigned getHRES()
{
	return h_res;
}

unsigned getVRES()
{
	return v_res;
}

unsigned getBitsPerPixel()
{
	return bits_per_pixel;
}

void colorPixel(int x, int y, char * video_mem, short color)
{
	video_mem += (y * h_res + x) * (bits_per_pixel / 8);
	*video_mem = (char) color;
}

void drawSlopingLine(int xi, int yi, int xf, int yf, short color)
{
	int aux_x, aux_y;

	if (xi > xf && yi > yf)
	{
		aux_x = xf;
		aux_y = yf;
		xf = xi;
		xi = aux_x;
		yf = yi;
		yi = aux_y;
	}

	if (xf < xi && yf > yi)
	{
		aux_x = xf;
		aux_y = yf;
		yf = yi;
		yi = aux_y;
		xf = xi;
		xi = aux_x;
	}

	//Line drawing algorithm
	double dx = xf - xi;
	double dy = yf - yi;

	int x, y;
	for (x = xi; x < xf; x++)
	{
		y = yi + dy * (x - xi) / dx;
		colorPixel(x, y, video_mem, color);
	}
}

void drawSprite(Sprite * s)
{
	int heighttmp, widthtmp;
	for (heighttmp = 0; heighttmp < s->height; heighttmp++)
	{
		for (widthtmp = 0; widthtmp < s->width; widthtmp++)
		{
			colorPixel((unsigned short) (widthtmp + s->x), (unsigned short) (heighttmp + s->y), video_mem ,s->map[(heighttmp * s->width + widthtmp) * bits_per_pixel / 8]);
		}
	}
}

void drawLine(int xi, int yi, int xf, int yf, char * video_mem, short color)
{
	if (xi != xf && yi != yf)
	{
		drawSlopingLine(xi, yi, xf, yf, color);
		return;
	} else if (xi == xf) //VERTICAL
	{
		int cnt = 0;
		int i = 0;
		while (cnt < (yf - yi))
		{
			colorPixel(xi, yi + i, video_mem, color);
			cnt++;
			i++;
		}
	} else //HORIZONTAL
	{
		int cnt = 0;
		int i = 0;
		while (cnt < (xf - xi))
		{
			colorPixel(xi + i, yi, video_mem, color);
			cnt++;
			i++;
		}
	}
	return;
}

int vg_exit()
{
	struct reg86u reg86;

	reg86.u.b.intno = VBE_INT10; /* BIOS video services */
	reg86.u.b.ah = VBE_SET_VIDEO_MODE; /* Set Video Mode function */
	reg86.u.b.al = VBE_SET_TEXT_MODE; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK)
	{
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	}
	return 0;
}

void clearScreen() {
	int height, width;
	int x = 0;
	int y = 0;

	for (height = 0; height < v_res; height++)
	{
		for (width = 0; width < h_res; width++)
		{
			colorPixel(x + width, y + height, video_mem, 0);
		}
	}
}


