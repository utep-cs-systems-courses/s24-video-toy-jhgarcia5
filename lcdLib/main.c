#include <msp430.h>
#include "libTimer.h"
#include "lcdutils.h"
#include "lcddraw.h"

#define SWITCHES ( BIT0 | BIT1 | BIT2 | BIT3 )
#define LED BIT6

int colors[4] = {COLOR_PINK, COLOR_PURPLE, COLOR_RED, COLOR_WHITE};
int colorIndex = 0;

int redrawScreen = 1;

static char switch_update_interrupt_sense()
{

  char p2val = P2IN;
  P2IES |= (p2val & SWITCHES);
  P2IES &= (p2val | ~SWITCHES);
  return p2val;

}

void switch_init()
{
  P2REN |= SWITCHES;
  P2IE |= SWITCHES;
  P2OUT |= SWITCHES;
  P2DIR &= ~SWITCHES;
  switch_update_interrupt_sense();
}

int switches = 0;

void switch_interrupt_handler()
{

  colorIndex = (colorIndex + 1) % 4;
  redrawScreen = 1;
  
  /*
  char p2val = switch_update_interrupt_sense();
  switches = ~p2val & SWITCHES;

  if (switches & SWITCHES){

    

  }
  */
}


void main()
{
  P1DIR |= LED;
  P1OUT |= LED;
  
  WDTCTL = WDTPW | WDTHOLD;
  configureClocks();
  lcd_init();
  switch_init();
  
  enableWDTInterrupts();
  or_sr(0x8);

  clearScreen(COLOR_BLACK);

  while(1) {
    if (redrawScreen) {
      redrawScreen = 0;
      clearScreen(colors[colorIndex]);
    }
    P1OUT &= ~LED;
    or_sr(0x10);
    P1OUT |= LED;
  }
}

void __interrupt_vec(PORT2_VECTOR) PORT_2()
{
  if (P2IFG & SWITCHES){
    P2IFG &= ~SWITCHES;
    switch_interrupt_handler();
  }
}
