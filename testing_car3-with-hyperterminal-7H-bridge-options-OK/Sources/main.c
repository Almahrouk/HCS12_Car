#include <hidef.h>
#include "derivative.h"

/*
  HCS12 Car Control from HyperTerminal (SCI0) + 7-Seg Debug on Port H

  HyperTerminal:
    9600, 8N1, No flow control
    Keys:
      w = forward
      s = backward
      a = left
      d = right
      x = stop

  7-seg debug (Port H):
    0 = STOP
    1 = FORWARD
    2 = BACKWARD
    3 = LEFT
    4 = RIGHT

  Motors:
    PWM:
      PP0 -> Right motor (Motor1)
      PP1 -> Left motor  (Motor2)
    Direction (Port B):
      Motor1: PB0/PB1
      Motor2: PB2/PB3
*/

#define PWM_MAX        255u
#define BASE_SPEED     200u
#define TURN_DROP      120u

/* 7-seg wiring:
   - If your display is active-low (common anode / inverted), keep 1
   - If active-high, set to 0
*/
#define SEG_ACTIVE_LOW  1

static const unsigned char segDigits[10] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66,
  0x6D, 0x7D, 0x07, 0x7F, 0x67
};

/* ===== Prototypes ===== */
void PLL_init(void);

/* SCI0 */
void SCI0_init_9600(void);
unsigned char SCI0_Rx(void);
void SCI0_Tx(unsigned char b);

/* PWM + Motors */
void PWM_init(void);
void setMotorSpeed(unsigned char right, unsigned char left);
void motorsStop(void);
void motorsForward(void);
void motorsBackward(void);

/* 7-seg */
static void seg_off(void);
static void seg_show_digit(unsigned char d);

/* UI */
static void printMenu(void);

/* ===== Main ===== */
void main(void)
{
  unsigned char cmd;

  PLL_init();
  SCI0_init_9600();
  PWM_init();

  /* Direction pins */
  DDRB = 0x0F;          /* PB0..PB3 outputs */
  motorsStop();
  setMotorSpeed(0, 0);

  /* 7-seg debug */
  DDRH = 0xFF;
  seg_off();
  seg_show_digit(0);    /* show STOP at boot */

  printMenu();

  for (;;)
  {
    cmd = SCI0_Rx();

    /* ignore CR/LF from terminal */
    if (cmd == '\r' || cmd == '\n') continue;

    switch (cmd)
    {
      case 'w':
      case 'W':
        motorsForward();
        setMotorSpeed(BASE_SPEED, BASE_SPEED);
        seg_show_digit(1);               /* FORWARD */
        break;

      case 's':
      case 'S':
        motorsBackward();
        setMotorSpeed(BASE_SPEED, BASE_SPEED);
        seg_show_digit(2);               /* BACKWARD */
        break;

      case 'a':
      case 'A':
        motorsForward();                 /* turning while moving forward */
        setMotorSpeed(BASE_SPEED, (BASE_SPEED > TURN_DROP) ? (BASE_SPEED - TURN_DROP) : 0);
        seg_show_digit(3);               /* LEFT */
        break;

      case 'd':
      case 'D':
        motorsForward();
        setMotorSpeed((BASE_SPEED > TURN_DROP) ? (BASE_SPEED - TURN_DROP) : 0, BASE_SPEED);
        seg_show_digit(4);               /* RIGHT */
        break;

      case 'x':
      case 'X':
      default:
        motorsStop();
        setMotorSpeed(0, 0);
        seg_show_digit(0);               /* STOP */
        break;
    }

    /* optional echo */
    SCI0_Tx(cmd);
    SCI0_Tx('\r'); SCI0_Tx('\n');
    SCI0_Tx('>'); SCI0_Tx(' ');

    _FEED_COP();
  }
}

/* ===== Clock ===== */
void PLL_init(void)
{
  SYNR   = 2;
  REFDV  = 0;
  PLLCTL = 0x60;
  while (!(CRGFLG & 0x08)) {}
  CLKSEL |= 0x80;
}

/* ===== SCI0 ===== */
void SCI0_init_9600(void)
{
  SCI0BDL = 156;
  SCI0BDH = 0;
  SCI0CR1 = 0;      /* 8N1 */
  SCI0CR2 = 0x0C;   /* TE, RE */
}

unsigned char SCI0_Rx(void)
{
  while (!(SCI0SR1 & 0x20)) {}  /* RDRF */
  return SCI0DRL;
}

void SCI0_Tx(unsigned char b)
{
  while (!(SCI0SR1 & 0x80)) {}  /* TDRE */
  SCI0DRL = b;
}

/* ===== PWM (PP0/PP1) ===== */
void PWM_init(void)
{
  PWME   = 0x03;     /* enable ch0, ch1 */
  PWMPOL = 0x03;     /* active-high pulses */
  PWMCTL = 0x00;     /* independent */
  PWMCLK = 0x00;     /* use clocks A/B */
  PWMPRCLK = 0x03;   /* prescaler (tune if needed) */

  PWMPER0 = (unsigned char)PWM_MAX;
  PWMPER1 = (unsigned char)PWM_MAX;

  PWMDTY0 = 0;
  PWMDTY1 = 0;
}

void setMotorSpeed(unsigned char right, unsigned char left)
{
  PWMDTY0 = right;   /* PP0 */
  PWMDTY1 = left;    /* PP1 */
}

/* ===== Motor direction (Port B) =====
   Forward = 0x05 (PB0=1,PB2=1)
   Backward=0x0A (PB1=1,PB3=1)
*/
void motorsStop(void)
{
  PORTB &= (unsigned char)~0x0F;  /* PB0..PB3 = 0 */
}

void motorsForward(void)
{
  PORTB = (PORTB & (unsigned char)~0x0F) | 0x05;
}

void motorsBackward(void)
{
  PORTB = (PORTB & (unsigned char)~0x0F) | 0x0A;
}

/* ===== 7-seg ===== */
static void seg_off(void)
{
#if SEG_ACTIVE_LOW
  PTH = 0xFF;
#else
  PTH = 0x00;
#endif
}

static void seg_show_digit(unsigned char d)
{
  unsigned char pat = segDigits[d % 10];
#if SEG_ACTIVE_LOW
  PTH = (unsigned char)~pat;
#else
  PTH = pat;
#endif
}

/* ===== UI ===== */
static void printMenu(void)
{
  const char *msg =
    "\r\nHCS12 Car Control (SCI0 @ 9600)\r\n"
    "w=FWD  s=BWD  a=LEFT  d=RIGHT  x=STOP\r\n"
    "7-seg: 0=STOP 1=FWD 2=BWD 3=LEFT 4=RIGHT\r\n> ";

  while (*msg) SCI0_Tx((unsigned char)*msg++);
}
