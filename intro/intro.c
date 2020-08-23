#include <lynx.h>
#include <tgi.h>
#include <joystick.h>
#include <stdlib.h>
#include <conio.h>
#include <6502.h>
#include <time.h>

unsigned char checkInput(void);
extern unsigned char reset; 

extern unsigned char bg[];

static SCB_REHV_PAL Sbg = {
    0xc0, 0x10, 0x01,
    0,
    bg,
    0, 0,
    0x100, 0x100,
    {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}
};

void intro(void)
{
    clock_t now = clock();
    
    do {
        while (tgi_busy())
            ;
	tgi_setcolor(COLOR_BLACK);
	tgi_bar(0,0,159,101);
        tgi_setcolor(COLOR_YELLOW);
        tgi_outtextxy(40, 51, "Tree Jumper");
        tgi_updatedisplay();
        checkInput(); // check input in every loop if you want that pause and reset event are handled
    } while (clock() - now < 2 * 75  && !reset); //every loop needs to be stopped if reset is triggered

}

