#include <hidef.h>
#include "derivative.h"

/* ====== Settings ====== */
#define SEG_ACTIVE_LOW  1   /* 1 if your 7-seg is active-low, 0 if active-high */

/* 7-seg encodings for digits 0..9 */
static const unsigned char segDigits[10] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66,
  0x6D, 0x7D, 0x07, 0x7F, 0x67
};

/* ====== Prototypes ====== */
void PLL_init(void);
void SCI0_init_9600(void);
unsigned char SCI0_Rx(void);
void SCI0_Tx(unsigned char b);

static void seg_off(void);
static void seg_put_raw(unsigned char pat);
static void seg_put_digit(unsigned char d);

static unsigned char hex_nibble(char c);
static unsigned char get_hex_byte_from_terminal_SCI0(void);

/* ====== Main ====== */
/*
HyperTerminal usage:
- 9600 baud, 8 data bits, No parity, 1 stop bit, No flow control.
- Send ONE byte as two HEX characters then Enter.
  Examples: 00, 7F, 80, FF
This code reads from SCI0, echoes back, and displays:
  MSB=0 -> "6"
  MSB=1 -> "3"
*/
void main(void)
{
  unsigned char v;

  PLL_init();
  SCI0_init_9600();

  DDRH = 0xFF;   /* Port H output */
  seg_off();

  /* Greeting */
  SCI0_Tx('\r'); SCI0_Tx('\n');
  SCI0_Tx('>');  SCI0_Tx(' ');

  for (;;)
  {
    v = get_hex_byte_from_terminal_SCI0();   /* read 00..FF from terminal */

    /* Echo back what was received */
    SCI0_Tx('R'); SCI0_Tx('x'); SCI0_Tx(':'); SCI0_Tx(' ');
    {
      const char hex[] = "0123456789ABCDEF";
      SCI0_Tx(hex[(v >> 4) & 0x0F]);
      SCI0_Tx(hex[v & 0x0F]);
      SCI0_Tx('\r'); SCI0_Tx('\n');
      SCI0_Tx('>'); SCI0_Tx(' ');
    }

    /* Display based on MSB */
    if (v & 0x80) seg_put_digit(3);  /* MSB=1 */
    else          seg_put_digit(6);  /* MSB=0 */

    _FEED_COP();
  }
}

/* ====== Clock ====== */
void PLL_init(void)
{
  SYNR   = 2;
  REFDV  = 0;
  PLLCTL = 0x60;
  while (!(CRGFLG & 0x08)) {}
  CLKSEL |= 0x80;
}

/* ====== SCI0 @ 9600, 8N1 ====== */
void SCI0_init_9600(void)
{
  SCI0BDL = 156;
  SCI0BDH = 0;      /* 9600 bps @ 24MHz E clock */
  SCI0CR1 = 0;      /* 8N1 */
  SCI0CR2 = 0x0C;   /* TE=1, RE=1 */
}

void SCI0_Tx(unsigned char b)
{
  while (!(SCI0SR1 & 0x80)) {}  /* TDRE */
  SCI0DRL = b;
}

unsigned char SCI0_Rx(void)
{
  while (!(SCI0SR1 & 0x20)) {}  /* RDRF */
  return SCI0DRL;
}

/* ====== 7-seg helpers ====== */
static void seg_put_raw(unsigned char pat)
{
#if SEG_ACTIVE_LOW
  PTH = (unsigned char)~pat;
#else
  PTH = pat;
#endif
}

static void seg_put_digit(unsigned char d)
{
  seg_put_raw(segDigits[d % 10]);
}

static void seg_off(void)
{
#if SEG_ACTIVE_LOW
  PTH = 0xFF;
#else
  PTH = 0x00;
#endif
}

/* ====== Terminal parsing: read 2 hex chars (00..FF) from SCI0 ====== */
static unsigned char hex_nibble(char c)
{
  if (c >= '0' && c <= '9') return (unsigned char)(c - '0');
  if (c >= 'A' && c <= 'F') return (unsigned char)(c - 'A' + 10);
  if (c >= 'a' && c <= 'f') return (unsigned char)(c - 'a' + 10);
  return 0xFF;
}

static unsigned char get_hex_byte_from_terminal_SCI0(void)
{
  char c;
  unsigned char n1 = 0xFF, n2 = 0xFF;

  while (1)
  {
    c = (char)SCI0_Rx();

    /* ignore CR/LF */
    if (c == '\r' || c == '\n') continue;

    /* ignore spaces */
    if (c == ' ' || c == '\t') continue;

    /* echo typed char */
    SCI0_Tx((unsigned char)c);

    if (n1 == 0xFF)
    {
      n1 = hex_nibble(c);
      if (n1 == 0xFF)
      {
        SCI0_Tx('\r'); SCI0_Tx('\n');
        SCI0_Tx('E'); SCI0_Tx('R'); SCI0_Tx('R');
        SCI0_Tx('\r'); SCI0_Tx('\n');
        SCI0_Tx('>'); SCI0_Tx(' ');
        n1 = 0xFF; n2 = 0xFF;
      }
    }
    else if (n2 == 0xFF)
    {
      n2 = hex_nibble(c);
      if (n2 == 0xFF)
      {
        SCI0_Tx('\r'); SCI0_Tx('\n');
        SCI0_Tx('E'); SCI0_Tx('R'); SCI0_Tx('R');
        SCI0_Tx('\r'); SCI0_Tx('\n');
        SCI0_Tx('>'); SCI0_Tx(' ');
        n1 = 0xFF; n2 = 0xFF;
      }
      else
      {
        SCI0_Tx('\r'); SCI0_Tx('\n');
        return (unsigned char)((n1 << 4) | n2);
      }
    }
  }
}
