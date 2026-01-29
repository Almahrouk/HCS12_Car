#include <hidef.h>
#include "derivative.h"

/* ===== Prototypes ===== */
void PLL_init(void);
void delay_ms_like(void);

void SCI_init(void);                 /* init SCI1 (and SCI0 if you want) */
unsigned char SCI1_Rx(void);

/* 7-seg digit encodings (common-cathode style). If yours is active-low, invert. */
static const unsigned char segDigits[10] =
{
  0x3F, /*0*/
  0x06, /*1*/
  0x5B, /*2*/
  0x4F, /*3*/
  0x66, /*4*/
  0x6D, /*5*/
  0x7D, /*6*/
  0x07, /*7*/
  0x7F, /*8*/
  0x67  /*9*/
};

/* If your display is active-low (very common with common-anode), keep this = 1 */
#define SEG_ACTIVE_LOW  1

static void display_digit(unsigned char d)
{
  unsigned char pat = segDigits[d % 10];

#if SEG_ACTIVE_LOW
  PTH = (unsigned char)~pat;
#else
  PTH = pat;
#endif
}

static void display_off(void)
{
#if SEG_ACTIVE_LOW
  PTH = 0xFF;
#else
  PTH = 0x00;
#endif
}

/* ===== Main ===== */
void main(void)
{
  unsigned char b1, b2;

  PLL_init();
  SCI_init();

  DDRH = 0xFF;     /* Port H outputs to 7-seg */
  display_off();

  for (;;)
  {
    /* Receive the TWO bytes sent by Arduino/Simulink each cycle */
    b1 = SCI1_Rx();    /* expected MSB=0 */
    b2 = SCI1_Rx();    /* expected MSB=1 */

    /* Show what arrived (quick visual debug) */
    if (b1 & 0x80) display_digit(9);  /* ERROR: byte1 should NOT have MSB=1 */
    else          display_digit(6);   /* OK: MSB=0 */
    delay_ms_like();

    if (b2 & 0x80) display_digit(3);  /* OK: MSB=1 */
    else          display_digit(6);   /* ERROR: byte2 arrived with MSB=0 */
    delay_ms_like();

    display_off();

    _FEED_COP();
  }
}

/* ===== Clock ===== */
void PLL_init(void)
{
  SYNR   = 2;
  REFDV  = 0;          /* 8MHz OSC -> 48MHz PLL (as in your original) */
  PLLCTL = 0x60;       /* PLL ON, Auto */
  while (!(CRGFLG & 0x08)) { }  /* wait for LOCK */
  CLKSEL |= 0x80;      /* select PLL clock */
}

/* ===== Simple Delay (busy wait) ===== */
void delay_ms_like(void)
{
  unsigned char j;
  unsigned int  k;

  for (j = 0; j < 80; j++)
  {
    for (k = 0; k < 20000; k++)
    {
      __asm nop;
    }
  }
}

/* ===== SCI ===== */
void SCI_init(void)
{
  /* SCI1 @ 9600 bps, 8N1, polling */
  SCI1BDL = 156;
  SCI1BDH = 0;
  SCI1CR1 = 0;
  SCI1CR2 = 0x0C;   /* TE=1, RE=1 */

  /* Optional: also init SCI0 the same way (not used here) */
  SCI0BDL = 156;
  SCI0BDH = 0;
  SCI0CR1 = 0;
  SCI0CR2 = 0x0C;
}

unsigned char SCI1_Rx(void)
{
  while (!(SCI1SR1 & 0x20)) { }  /* RDRF */
  return SCI1DRL;
}
