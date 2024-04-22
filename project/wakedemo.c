#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

// WARNING: LCD DISPLAY USES P1.0.  Do not touch!!! 

#define LED BIT6		/* note that bit zero req'd for display */

#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8

#define SWITCHES 15

char blue = 31, green = 0, red = 31;
unsigned char step = 0;

int size = 10;
u_char width = screenWidth;
u_char height = screenHeight;

short centerCol = screenWidth >> 1;
short centerRow = screenHeight >> 1;

short redrawScreen = 1;

unsigned short heartColors[4] = {COLOR_PINK, COLOR_PURPLE, COLOR_RED, COLOR_MAGENTA};
int heartColorIndex = 0;

unsigned short backgroundColors[4] = {COLOR_ORANGE, COLOR_YELLOW, COLOR_BLUE, COLOR_PURPLE};
int bgColorIndex = 0;

int lineStart = screenHeight;

int debounceTimer = 0;

static char 
switch_update_interrupt_sense()
{
  char p2val = P2IN;
  /* update switch interrupt to detect changes from current buttons */
  P2IES |= (p2val & SWITCHES);	/* if switch up, sense down */
  P2IES &= (p2val | ~SWITCHES);	/* if switch down, sense up */
  return p2val;
}

void 
switch_init()			/* setup switch */
{  
  P2REN |= SWITCHES;		/* enables resistors for switches */
  P2IE |= SWITCHES;		/* enable interrupts from switches */
  P2OUT |= SWITCHES;		/* pull-ups for switches */
  P2DIR &= ~SWITCHES;		/* set switches' bits for input */
  switch_update_interrupt_sense();
}

int switches = 0;
int state = 0;

void
switch_interrupt_handler()
{
  
  char p2val = switch_update_interrupt_sense();
  switches = ~p2val & SWITCHES;

  if (switches & SW1){
    size += 1;
    state = 0;
  }

  if (switches & SW2) {
    if(size != 1){
      size -= 1;
      state = 0;
    }
  }

  if(switches & SW3) {
    heartColorIndex = (heartColorIndex + 1) % 4;
    state = 0;
  }

  if (switches & SW4) {
    state = 1;
    debounceTimer = 5;
  }

  redrawScreen = 1;
  
}


// axis zero for col, axis 1 for row

short drawPos[2] = {1,10}, controlPos[2] = {2, 10};
short colVelocity = 1, colLimits[2] = {1, screenWidth/2};

void
draw_ball(int col, int row, unsigned short color)
{
  fillRectangle(col-1, row-1, 3, 3, color);
}


void draw_heart(int col, int row, int size, unsigned short color)
{
  for(short i = 0; i < size/2; i++){
    drawPixel(centerCol + i, centerRow - i, color);
    drawPixel(centerCol - i, centerRow - i, color);
  }

  for(short i = 0; i < size/2; i++){
    drawPixel(centerCol-(size/2)-i, centerRow - (size/2) + i, color);
    drawPixel(centerCol+(size/2)+i, centerRow - (size/2) + i, color);
  }

  for (short i = 0; i < size; i++) {
    drawPixel(centerCol - i + size, centerRow + i, color);
    drawPixel(centerCol + i - size, centerRow + i, color);
  }
}

void custom_state()
{
  //clearScreen(COLOR_MAGENTA);
}

void draw_ground()
{
  fillRectangle(0, centerRow + 20, width, 100, COLOR_GREEN); //GRass
  fillRectangle(centerCol-30, centerRow + 18, 60, 100, COLOR_GRAY); //Road
}

void draw_line(int y, unsigned short color)
{
  fillRectangle(centerCol - 3, y, centerCol + 2, y + 2, color);
}

void draw_ambulance()
{
  //Ambulance
  fillRectangle(centerCol - 20, centerRow + 5, 40, 30, COLOR_WHITE); //BottomBodyPart
  fillRectangle(centerCol - 15, centerRow - 5, 30, 10, COLOR_WHITE); //TopBodyPart
  fillRectangle(centerCol - 10, centerRow - 3, 20, 9, COLOR_BLUE); //Wind shield
  fillRectangle(centerCol - 25, centerRow + 30, 8, 10, COLOR_BLACK); //Left wheel
  fillRectangle(centerCol + 17, centerRow + 30, 8, 10, COLOR_BLACK); //Left right
}

void update_line()
{
  draw_line(lineStart - 2, COLOR_RED);
  draw_line(lineStart, COLOR_YELLOW);
  lineStart -= 2;
  
  if(lineStart <= centerRow + 30){
    lineStart = screenHeight;
  }
}

void changeBackground(unsigned short color)
{
  //clearScreen(color);
  fillRectangle(0,0,width,centerRow - 5,color); // Above ambulance
  fillRectangle(0,centerRow - 5, 49, 10, color); // Left side top
  fillRectangle(79,centerRow - 5,100, 10, color); //Right side top

  fillRectangle(0, centerRow, 45, 18, color); //Left side bottom
  fillRectangle(83, centerRow, 100, 18, color); //Right side bottom

  fillRectangle(0, centerRow + 18, 34, 2, color);
  fillRectangle(94, centerRow + 18, 100, 2, color);
}

void
screen_update_ball()
{
  for (char axis = 0; axis < 2; axis ++) 
    if (drawPos[axis] != controlPos[axis]) /* position changed? */
      goto redraw;
  return;			/* nothing to do */
 redraw:
  draw_ball(drawPos[0], drawPos[1], COLOR_BLUE); /* erase */
  for (char axis = 0; axis < 2; axis ++) 
    drawPos[axis] = controlPos[axis];
  draw_ball(drawPos[0], drawPos[1], COLOR_WHITE); /* draw */
}
  
u_int controlFontColor = COLOR_GREEN;

void wdt_c_handler()
{
  static int secCount = 0;
  
  secCount ++;
  if (secCount >= 250) {		/* 10/sec */
   
    if (state == 1){

      if (secCount == 250){
	bgColorIndex = (bgColorIndex + 1) % 4;
	update_line();
	redrawScreen = 1; 
      }
      /*
      if (secCount % 20 == 0) {
	update_line();
	redrawScreen = 1;
      }
      */
      
      //bgColorIndex = (bgColorIndex + 1) % 4;
      //redrawScreen = 1;
      //clearScreen(backgroundColors[bgColorIndex]);
    }
// {				/* move ball */
//    short oldCol = controlPos[0];
//    short newCol = oldCol + colVelocity;
//    if (newCol <= colLimits[0] || newCol >= colLimits[1])
//	colVelocity = -colVelocity;
//    else
//	controlPos[0] = newCol;
//  }

//  {				/* update hourglass */
//    if (switches & SW3) green = (green + 1) % 64;
//    if (switches & SW2) blue = (blue + 2) % 32;
//    if (switches & SW1) red = (red - 3) % 32;
//     if (step <= 30)
//	step ++;
//    else
//	step = 0;
    secCount = 0;
//  }
//  if (switches & SW4) return;
    redrawScreen = 1;
  }
}
  
void update_shape();

void main()
{
  
  P1DIR |= LED;		/**< Green led on when CPU on */
  P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
  
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  
  clearScreen(COLOR_BLUE);
  int drawGround = 1;
  
  while (1) {			/* forever */
    if (redrawScreen) {
      redrawScreen = 0;

      switch(state){
      case 0:
	drawGround = 1;
	custom_state();
	update_shape();
	break;
	
      case 1:

	if (drawGround){
	    draw_ground();
	    drawGround = 0;
	    update_line();
	}
	changeBackground(backgroundColors[bgColorIndex]);
	draw_ambulance();
	break;
	
      default:
	break;
      }
    }

    if (debounceTimer > 0) {
      debounceTimer -= 1;
    }
    
    P1OUT &= ~LED;	/* led off */
    or_sr(0x10);	/**< CPU OFF */
    P1OUT |= LED;	/* led on */
  }
}

void
screen_update_hourglass()
{
  static unsigned char row = screenHeight / 2, col = screenWidth / 2;
  static char lastStep = 0;
  
  if (step == 0 || (lastStep > step)) {
    clearScreen(COLOR_BLUE);
    lastStep = 0;
  } else {
    for (; lastStep <= step; lastStep++) {
      int startCol = col - lastStep;
      int endCol = col + lastStep;
      int width = 1 + endCol - startCol;
      
      // a color in this BGR encoding is BBBB BGGG GGGR RRRR
      unsigned int color = (blue << 11) | (green << 5) | red;
      
      fillRectangle(startCol, row+lastStep, width, 1, color);
      fillRectangle(startCol, row-lastStep, width, 1, color);
    }
  }
}  


    
void
update_shape()
{
  draw_heart(centerCol, centerRow, size + 1, COLOR_BLUE);
  draw_heart(centerCol, centerRow, size, heartColors[heartColorIndex]);
  //screen_update_ball();
  // screen_update_hourglass();
}
   


void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {	      /* did a button cause this interrupt? */
    P2IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}
