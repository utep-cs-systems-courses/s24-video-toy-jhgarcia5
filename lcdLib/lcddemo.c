/** \file lcddemo.c
 *  \brief A simple demo that draws a string and square
 */

#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

/** Initializes everything, clears the screen, draws "hello" and a square */
int
main()
{
  configureClocks();
  lcd_init();
  
  u_char width = screenWidth, height = screenHeight;  
  // drawString5x7(20,20, "hello", COLOR_GREEN, COLOR_RED);

  //fillRectangle(30,30, 60, 60, COLOR_ORANGE);

  
  short centerCol = screenWidth >> 1;
  short centerRow = screenHeight >> 1;

  short size = 20;
  //Top part of heart
  for (short i = 0; i < size/2; i++){
    drawPixel(centerCol+i, centerRow - i, COLOR_PINK);
    drawPixel(centerCol-i, centerRow -i, COLOR_PINK);
  }

  //TOP PART, edges

  for (short i=0; i < size/2; i++){
    drawPixel(centerCol-(size/2)-i, centerRow-(size/2)+i,COLOR_PINK);
    drawPixel(centerCol+(size/2)+i, centerRow-(size/2)+i, COLOR_PINK);      
  }

  //BOTTOM PART

  for (short i = 0; i < size; i++){
    drawPixel(centerCol - size + i, centerRow + i, COLOR_PINK);
    drawPixel(centerCol + size - i, centerRow + i, COLOR_PINK);
  }  
  /*
  int x = 0;
  int y = 0;
  int speed = 6;
  for (short i = 0; i < screenWidth; i++){
    drawPixel(x, centerRow - y, COLOR_PINK);
    drawPixel(x, centerRow - speed, COLOR_GREEN);
    x++;
    y += speed;
    speed -= 1;
    if (y <= 0)
      speed =- speed;
  }

  */
}
