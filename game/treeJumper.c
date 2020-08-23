#include <lynx.h>
#include <conio.h>
#include <joystick.h>
#include <tgi.h>
#include <stdlib.h>

unsigned char checkInput(void);
extern unsigned char reset; 

/* Start of Sound Setup (For use with Chipper Sounds */
void lynx_snd_init ();
void lynx_snd_pause ();
void lynx_snd_continue ();
void __fastcall__ lynx_snd_play (unsigned char channel, unsigned char *music);
void lynx_snd_stop ();

typedef struct
    {
    unsigned char *music0;
    unsigned char *music1;
    unsigned char *music2;
    unsigned char *music3;
    } song_t;

//Declare the sound object
extern song_t musicptr;
/* End of Sound Setup */

/* Set up the two sprites */
//Declare the tree sprite object
extern unsigned char tree[];

// Define the tree sprite
static SCB_REHV_PAL Stree =
    {
    BPP_4 | TYPE_NORMAL,
    0x10, 0x20,
    0,
    tree,
    0, 0,
    0x0100, 0x100,
    {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}
    };

//Declare the boy sprite
extern unsigned char dude[];

// Define the by sprite
static SCB_REHV_PAL Sdude =
    {
    BPP_4 | TYPE_NORMAL,
    0x10, 0x20,
    0,
    dude,
    0, 0,
    0x0100, 0x100,
    {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}
    };

// Function to draw the tree at x, y with a certain scale
void draw_tree (int x, int y, int scale)
    {
    Stree.vpos = y;
    Stree.hpos = x;
    Stree.vsize = scale;
    Stree.hsize = scale;
    tgi_sprite(&Stree);
    }

// Function to draw the boy at dx, dy, with a certain scale
void draw_dude (int dx, int dy, int scale)
    {
    Sdude.vpos = dy;
    Sdude.hpos = dx;
    Sdude.vsize = scale;
    Sdude.hsize = scale;
    tgi_sprite(&Sdude);
    }

// Function to draw the screen
char drawscreen(char x, char y, char dx, char dy, int score, char lives)
    {
// Declare variables for textscore, textlives, and gameOver
    char textScore[20];
    char textLives[20];
// Set the color to black    
    tgi_setcolor(COLOR_BLACK);
// Draw a bar that covers the entire screen (the background) 
    tgi_bar(0, 0, 159, 101);
// Set the color to brown
    tgi_setcolor(COLOR_BROWN);
// Draw bar that represents the ground
    tgi_bar(25, 86, 159, 88);
// Draw the small tree and boy to indicate score and lives
    draw_tree(110, 14, 18);
    draw_dude(12, 3, 18);
//Convert numerical score and lives to text strings    
    itoa(score, textScore, 10);
    itoa(lives, textLives, 10);
// Set the color to yellow
    tgi_setcolor(COLOR_YELLOW);
// Print the score and lives at specific coordinates
    tgi_outtextxy(120, 10, textScore);
    tgi_outtextxy(25, 10, textLives);
// Draw the main boy and tree sprites
    draw_tree(x, y, 33);
    draw_dude(dx, dy, 33);
// If no more lives, then print GAME OVER, pause the sound, and return 1 to the main while loop
    if (lives == 0)
        {
        tgi_outtextxy(40, 51, "GAME OVER");
        lynx_snd_pause ();
        return(1);
        }
    else
        {
        return(0);
        }
    }

void game()
    {
// Using char instead of int for all numbers less than 255. char takes less memory than int.
    char tx = 170; // tree x coordinate
    char ty = 81; // tree y coordinate
    char dx = 25; // boy(dude) x coordinate
    char dy = 59; // boy (dude y coordinate
    char jump = 0;
    char jumpUp = 25;
    char jumpDown = 15;
    char dudeFall = 0;
    char collision = 0;
    char treeNew = 1;
    char lives = 3;
    char gameOver = 0;
    char collsound = 0;
    char fallsound = 0;
    int score = 0;
    
// Begin main while loop
    while (!reset)
        { // never use while(1). Every loop needs to be halted by the reset event
        if (!tgi_busy())
            {
/* Start of joypad detection */                
// Declare joy variable to capture joypad input
           char joy;
            
// Instead of calling joy_read() directly, use checkInput() defined in resident.c . 
// It will return the joy state, but will handle pause and reset events too

// In this game, if there's been a collision or if boy is already falling, no joypad presses are registered            
            if (collision == 0 && dudeFall == 0)
                {
                joy = checkInput();
// Up and Down aren't used in this game
                if (JOY_BTN_UP(joy))
                    {
                    }    
                if (JOY_BTN_DOWN(joy))
                    {
                    }
// Move the boy right, but don't let him go off the right side of the screen
                if (JOY_BTN_RIGHT(joy))
                    {
                    ++dx;
                    if (dx > 140) dx = 140;
                    }
                if (JOY_BTN_LEFT(joy))
                    {
                    --dx;
                    }
                if (JOY_BTN_FIRE(joy) && jump == 0)
                    {
                    jump = 1;
                    lynx_snd_play(1, musicptr.music3);
                    }
// Start moving the boy up if Jump has been set, and to regulate the distance, count down from jumpUp
                if (jump == 1 && jumpUp > 0)
                    {
// This code cause the boy to move up most of the time, but to pause at the top for a moment
                    if (jumpUp > 10)
                        {    
                        dy = dy - 2;
                        }
                    --jumpUp;
                    }
// jumpUp is now at 0, so use the same process to bring by back down, as long as the boy is not on top of tree.                    
                if (jump ==1 && jumpUp == 0 && jumpDown > 0 && (dx < tx - 20 || dx > tx + 10))
                    {
                    dy = dy +2;
                    jumpDown--;
                    }
// Jump is complete, so reset the jump flag and variables for next jump
                if (jumpDown == 0)
                    {
                    jump = 0;
                    jumpUp = 25;
                    jumpDown = 15;
                    }
// Button B is not used in this game
                if (JOY_BTN_FIRE2(joy))
                    {
                    }    
                }
/* End of joypad detection */

/* Start of coordinate movements and collision and fall events */
// If tree is at the extreme left, make it fall off the screen.
            if (tx <= 10)
                {
                ++ty;
                }
// Otherwise, move it one coordinate to the left.            
            else
                {
                --tx;
                }
// If boy is at extreme left, make him fall.                
            if (dx <= 10)
                {
                dudeFall = 1;
                ++dy;
// If falling sound effect has not already been played, play it.
                if (fallsound == 0)
                    {
                    lynx_snd_play(1, musicptr.music1);
                    fallsound = 1;
                    }
                }
// If coordinates indicate that the tree has hit the boy, move the boy left and set collision flag
            if (dx >= tx-26 && dx <= tx-24 && dx > 0 && dy > 40)
                {
                --dx;
                collision = 1;
// If collision sound hasn't been played, play it.
                if (collsound == 0)
                    {
                    lynx_snd_play(1,musicptr.music2);
                    collsound = 1;
                    }    
                }
// If boy has moved past tree and is on the ground, and if it's a new tree, increment score.                
            if (dx > tx && dy == 59 && treeNew == 1)
                {
                ++score;
                treeNew = 0;
                }
// If tree has fallen completely off screen, reset it's variables.                
            if (ty >= 115)
                {
                tx = 170;
                ty = 81;
                treeNew = 1;
                }
// If boy has fallen completely off screen, reset his variables and reduce lives.
            if (dy >= 135)
                {
                dx = 25;
                dy = 59;
                dudeFall = 0;
                collision = 0;
                collsound = 0;
                fallsound = 0;
                jump = 0;
                jumpUp = 25;
                jumpDown = 15;
                --lives;
                }
// If a 1 was not returned from the last drawscreen function, call drawscreen again. Otherwise, the while loop continues.
            if (gameOver == 0)
                {
                gameOver = drawscreen(tx, ty, dx, dy, score, lives);
                tgi_updatedisplay();
                }
            }
        } // end main while loop
    } // end main game subroutine