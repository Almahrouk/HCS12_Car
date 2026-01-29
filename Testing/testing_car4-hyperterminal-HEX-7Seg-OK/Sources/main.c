#include <hidef.h>
#include "derivative.h"

/* ===================== Constants (NO #define) ===================== */
const unsigned char PWM_MAX = 235;

const unsigned char ZONE_LOW_END     = 60;
const unsigned char ZONE_NEUTRAL_L   = 61;
const unsigned char ZONE_NEUTRAL_H   = 67;
const unsigned char ZONE_HIGH_START  = 68;

/* Direction values (NO enum) */
const unsigned char DIR_OFF = 0;
const unsigned char DIR_CW  = 1;
const unsigned char DIR_CCW = 2;

/* ===================== Prototypes ===================== */
void PLL_init(void);

void SCI0_init(void);
void SCI0_Tx(unsigned char b);
unsigned char SCI0_Rx(void);
void PrintStringSCI0(unsigned char *msg);
void SCI0_TxHex(unsigned char b);

void PWM_init_motors(void);
void Motor1_set(unsigned char dir, unsigned char speed); /* Right motor */
void Motor2_set(unsigned char dir, unsigned char speed); /* Left motor  */

unsigned char map_forward_backward(unsigned char fb_byte, unsigned char *dir_out);
unsigned char map_left_right(unsigned char lr_byte, signed char *steer_sign_out);

void SEG7_init(void);
void SEG7_show_1(void);
void SEG7_show_4(void);

/* Terminal input helpers (SCI0) */
unsigned char hex_nibble(unsigned char c);
unsigned char SCI0_RxHexByte(void);

/* ===================== MAIN ===================== */
void main(void)
{
  unsigned char rx = 0;
  unsigned char b_fb = 0;     /* MSB=0 forward/back byte */
  unsigned char b_lr = 0x80;  /* MSB=1 left/right byte (default neutral with MSB set) */

  unsigned char fb_dir = 0;
  unsigned char fb_speed = 0;

  signed char steer_sign = 0; /* -1 left, +1 right, 0 none */
  unsigned char steer_mag = 0;

  unsigned char right_speed = 0;
  unsigned char left_speed  = 0;

  unsigned char *msg0 = "=== HCS12 Car Controller (SCI0 from HyperTerminal) ===\r\n.";
  unsigned char *msg1 = "Type HEX bytes like: 00 7F 80 FF (2 hex digits + Enter)\r\n.";

  PLL_init();
  SEG7_init();
  SCI0_init();      /* HyperTerminal on SCI0 */

  /* Port B: PB1..PB4 outputs (your wiring) */
  DDRB |= 0x1E;      /* 0001 1110: PB1..PB4 outputs */
  PORTB &= (unsigned char)~0x1E;

  PWM_init_motors();

  PrintStringSCI0(msg0);
  PrintStringSCI0(msg1);

  Motor1_set(DIR_OFF, 0);
  Motor2_set(DIR_OFF, 0);

  for(;;)
  {
    /*
      Receive one byte from HyperTerminal in HEX (00..FF).
      This replaces SCI1_Rx() from Arduino.
    */
    PrintStringSCI0((unsigned char*)"RX(hex)=0x");
    rx = SCI0_RxHexByte();
    SCI0_TxHex(rx);
    SCI0_Tx('\r');
    SCI0_Tx('\n');

    /* Same logic as your original: accept bytes in any order using MSB */
    if((rx & 0x80) == 0x00)
    {
      b_fb = rx;           /* forward/back */
      SEG7_show_4();       /* debug: got MSB0 */
    }
    else
    {
      b_lr = rx;           /* left/right */
      SEG7_show_1();       /* debug: got MSB1 */
    }

    fb_speed  = map_forward_backward(b_fb, &fb_dir);
    steer_mag = map_left_right(b_lr, &steer_sign);

    /* --- Apply motion --- */
    if(fb_dir == DIR_OFF)
    {
      /* No forward/back => either stop or rotate in place by steering */
      if(steer_sign == 0 || steer_mag == 0)
      {
        Motor1_set(DIR_OFF, 0);
        Motor2_set(DIR_OFF, 0);
      }
      else
      {
        /* Spin in place */
        if(steer_sign > 0)       /* right */
        {
          Motor1_set(DIR_CCW, steer_mag);
          Motor2_set(DIR_CW,  steer_mag);
        }
        else                      /* left */
        {
          Motor1_set(DIR_CW,  steer_mag);
          Motor2_set(DIR_CCW, steer_mag);
        }
      }
    }
    else
    {
      /* Moving forward/back: differential steering */
      unsigned int tmp;
      right_speed = fb_speed;
      left_speed  = fb_speed;

      if(steer_sign != 0 && steer_mag != 0)
      {
        if(steer_sign > 0)
        {
          /* Turn RIGHT: slow right, speed up left */
          right_speed = (right_speed > steer_mag) ? (unsigned char)(right_speed - steer_mag) : 0;

          tmp = (unsigned int)left_speed + (unsigned int)steer_mag;
          left_speed = (tmp > (unsigned int)PWM_MAX) ? PWM_MAX : (unsigned char)tmp;
        }
        else
        {
          /* Turn LEFT: slow left, speed up right */
          left_speed = (left_speed > steer_mag) ? (unsigned char)(left_speed - steer_mag) : 0;

          tmp = (unsigned int)right_speed + (unsigned int)steer_mag;
          right_speed = (tmp > (unsigned int)PWM_MAX) ? PWM_MAX : (unsigned char)tmp;
        }
      }

      Motor1_set(fb_dir, right_speed);
      Motor2_set(fb_dir, left_speed);
    }

    _FEED_COP();
  }
}

/* ===================== Mapping Functions ===================== */

unsigned char map_forward_backward(unsigned char fb_byte, unsigned char *dir_out)
{
  unsigned char reading;
  unsigned int speed;

  reading = (unsigned char)(fb_byte & 0x7F);

  if(reading <= ZONE_LOW_END)            /* 0..60 -> Backward */
  {
    speed = (unsigned int)(62U - (unsigned int)reading) * (unsigned int)PWM_MAX / 62U;
    *dir_out = DIR_CCW;                  /* adjust if wiring inverted */
    return (unsigned char)speed;
  }
  else if(reading >= ZONE_NEUTRAL_L && reading <= ZONE_NEUTRAL_H) /* 61..67 */
  {
    *dir_out = DIR_OFF;
    return 0;
  }
  else                                   /* 68..127 -> Forward */
  {
    speed = (unsigned int)((unsigned int)reading - (unsigned int)ZONE_HIGH_START)
            * (unsigned int)PWM_MAX
            / (127U - (unsigned int)ZONE_HIGH_START);
    *dir_out = DIR_CW;
    return (unsigned char)speed;
  }
}

unsigned char map_left_right(unsigned char lr_byte, signed char *steer_sign_out)
{
  unsigned char reading;
  unsigned int speed;

  reading = (unsigned char)(lr_byte & 0x7F);

  if(reading <= ZONE_LOW_END)            /* 0..60 -> Right */
  {
    speed = (unsigned int)(62U - (unsigned int)reading) * (unsigned int)PWM_MAX / 62U;
    *steer_sign_out = 1;
    return (unsigned char)speed;
  }
  else if(reading >= ZONE_NEUTRAL_L && reading <= ZONE_NEUTRAL_H) /* 61..67 */
  {
    *steer_sign_out = 0;
    return 0;
  }
  else                                   /* 68..127 -> Left */
  {
    speed = (unsigned int)((unsigned int)reading - (unsigned int)ZONE_HIGH_START)
            * (unsigned int)PWM_MAX
            / (127U - (unsigned int)ZONE_HIGH_START);
    *steer_sign_out = -1;
    return (unsigned char)speed;
  }
}

/* ===================== Motor Control (your PB mapping) ===================== */
/* Motor1 dir: PB1/PB2, PWM0 */
void Motor1_set(unsigned char dir, unsigned char speed)
{
  PORTB &= (unsigned char)~0x06;  /* clear PB1,PB2 */
  if(dir == DIR_OFF) { }
  else if(dir == DIR_CW)          /* PB1=1 */
    PORTB |= 0x02;
  else                            /* PB2=1 */
    PORTB |= 0x04;

  PWMDTY0 = speed;
}

/* Motor2 dir: PB3/PB4, PWM1 */
void Motor2_set(unsigned char dir, unsigned char speed)
{
  PORTB &= (unsigned char)~0x18;  /* clear PB3,PB4 */
  if(dir == DIR_OFF) { }
  else if(dir == DIR_CW)          /* PB3=1 */
    PORTB |= 0x08;
  else                            /* PB4=1 */
    PORTB |= 0x10;

  PWMDTY1 = speed;
}

/* ===================== PWM Init ===================== */
void PWM_init_motors(void)
{
  PWMCLK &= (unsigned char)~0x03;  /* use Clock A for PWM0/PWM1 */
  PWMPOL |= 0x03;                  /* active high */
  PWMPRCLK = 0x07;                 /* E/128 */
  PWMSCLA  = 0x04;                 /* /8 */
  PWMCTL   = 0x0C;                 /* 8-bit mode */
  PWMPER0  = PWM_MAX;
  PWMPER1  = PWM_MAX;
  PWMDTY0  = 0;
  PWMDTY1  = 0;
  PWMCNT0  = 0;
  PWMCNT1  = 0;
  PWME    |= 0x03;
}

/* ===================== PLL ===================== */
void PLL_init(void)
{
  SYNR = 2;
  REFDV = 0;
  PLLCTL = 0x60;
  while(!(CRGFLG & 0x08)) { }
  CLKSEL |= 0x80;
}

/* ===================== SCI0 ===================== */
void SCI0_init(void)
{
  SCI0BDL = 156;
  SCI0BDH = 0;
  SCI0CR1 = 0;
  SCI0CR2 = 0x0C;  /* Enable Tx, Rx */
}

void SCI0_Tx(unsigned char myByte)
{
  while(!(SCI0SR1 & 0x80)) { }
  SCI0DRL = myByte;
}

unsigned char SCI0_Rx(void)
{
  while(!(SCI0SR1 & 0x20)) { }
  return SCI0DRL;
}

void PrintStringSCI0(unsigned char *msg)
{
  unsigned long i = 0;
  while(msg[i] != '.')
  {
    SCI0_Tx(msg[i]);
    i++;
  }
}

void SCI0_TxHex(unsigned char b)
{
  const char hex[] = "0123456789ABCDEF";
  SCI0_Tx(hex[(b >> 4) & 0x0F]);
  SCI0_Tx(hex[b & 0x0F]);
}

/* ===================== 7-Seg ===================== */
void SEG7_init(void)
{
  DDRA = 0xFF;
  PORTA = 0x00;
}

/* gfedcba (bit0=a ... bit6=g) */
void SEG7_show_1(void) { PORTA = 0x06; }  /* b,c */
void SEG7_show_4(void) { PORTA = 0x66; }  /* b,c,f,g */

/* ===================== HyperTerminal HEX Byte Receiver ===================== */
/* Reads two hex characters (00..FF). Ignores spaces and CR/LF. */
unsigned char hex_nibble(unsigned char c)
{
  if(c >= '0' && c <= '9') return (unsigned char)(c - '0');
  if(c >= 'A' && c <= 'F') return (unsigned char)(c - 'A' + 10);
  if(c >= 'a' && c <= 'f') return (unsigned char)(c - 'a' + 10);
  return 0xFF; /* invalid */
}

unsigned char SCI0_RxHexByte(void)
{
  unsigned char c;
  unsigned char n1 = 0xFF;
  unsigned char n2 = 0xFF;

  while(1)
  {
    c = SCI0_Rx();

    /* ignore CR/LF and spaces */
    if(c == '\r' || c == '\n' || c == ' ' || c == '\t') continue;

    /* echo typed character */
    SCI0_Tx(c);

    if(n1 == 0xFF)
    {
      n1 = hex_nibble(c);
      if(n1 == 0xFF)
      {
        PrintStringSCI0((unsigned char*)"\r\nERR\r\n.");
        n1 = 0xFF; n2 = 0xFF;
      }
    }
    else
    {
      n2 = hex_nibble(c);
      if(n2 == 0xFF)
      {
        PrintStringSCI0((unsigned char*)"\r\nERR\r\n.");
        n1 = 0xFF; n2 = 0xFF;
      }
      else
      {
        PrintStringSCI0((unsigned char*)"\r\n.");
        return (unsigned char)((n1 << 4) | n2);
      }
    }
  }
}
