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

void PLL_init(void);
void SCI0_init(void);
void SCI0_Tx(unsigned char b);
void PrintStringSCI0(unsigned char *msg);
void SCI1_init(void);
unsigned char SCI1_Rx(void);
void PWM_init_motors(void);
void Motor1_set(unsigned char dir, unsigned char speed); /* Right motor */
void Motor2_set(unsigned char dir, unsigned char speed); /* Left motor  */
unsigned char map_forward_backward(unsigned char fb_byte, unsigned char *dir_out);
unsigned char map_left_right(unsigned char lr_byte, signed char *steer_sign_out);

void main(void)
{
	unsigned char rx = 0;
	unsigned char b_fb = 0;			// The is the byte for forward/backward MSB=0 for speed
	unsigned char b_lr = 0;  		// The is the byte for left/right     MSB=1 for steering
	unsigned char fb_dir = 0;
	unsigned char fb_speed = 0;
	signed char steer_sign = 0; /* -1 left, +1 right, 0 none */
	unsigned char steer_mag = 0;
	unsigned char right_speed = 0;
	unsigned char left_speed  = 0;
	unsigned char *msg0 = "=== HCS12 Car Controller (SCI1 + PWM + H-Bridge) ===\r\n.";
	unsigned char *msg1 = "Receiving 2 bytes: MSB0=F/B, MSB1=L/R\r\n.";

	PLL_init();
	SCI0_init();      /* Hyberterminal optional debug */
	SCI1_init();      /* Arduino link */

	/* Port B: PB0..PB3 output for motor direction */
	DDRB |= 0x0F;
	PORTB &= ~0x0F;
	PWM_init_motors();
	EnableInterrupts;
	PrintStringSCI0(msg0);
	PrintStringSCI0(msg1);
	Motor1_set(DIR_OFF, 0);
	Motor2_set(DIR_OFF, 0);

	for(;;)
	{
		/* Read one byte at a time; accept in any order */
		rx = SCI1_Rx();
		if((rx & 0x80) == 0x00)
			b_fb = rx; /* forward/back */
		else
			b_lr = rx; /* left/right */

		fb_speed = map_forward_backward(b_fb, &fb_dir);
		steer_mag = map_left_right(b_lr, &steer_sign);
		if(fb_dir == DIR_OFF)
		{
			if(steer_sign == 0 || steer_mag == 0)
			{
				Motor1_set(DIR_OFF, 0);
				Motor2_set(DIR_OFF, 0);
			}
			else
			{
				if(steer_sign > 0)
				{
					Motor1_set(DIR_CCW, steer_mag);
					Motor2_set(DIR_CW,  steer_mag);
				}
				else
				{
					Motor1_set(DIR_CW,  steer_mag);
					Motor2_set(DIR_CCW, steer_mag);
				}
			}
		}
		else
		{
			unsigned int tmp;
			right_speed = fb_speed;
			left_speed  = fb_speed;
			if(steer_sign != 0 && steer_mag != 0)
			{
				if(steer_sign > 0)
				{
					/* Turn RIGHT: slow right, speed up left */
					right_speed = (right_speed > steer_mag) ? (unsigned char)(right_speed - steer_mag) : 0;
					{
						tmp = (unsigned int)left_speed + (unsigned int)steer_mag;
						left_speed = (tmp > (unsigned int)PWM_MAX) ? PWM_MAX : (unsigned char)tmp;
					}
				}
				else
				{
					/* Turn LEFT: slow left, speed up right */
					left_speed = (left_speed > steer_mag) ? (unsigned char)(left_speed - steer_mag) : 0;
					{
						tmp = (unsigned int)right_speed + (unsigned int)steer_mag;
						right_speed = (tmp > (unsigned int)PWM_MAX) ? PWM_MAX : (unsigned char)tmp;
					}
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
	unsigned int speed;				// speed 0..PWM_MAX

	reading = fb_byte & 0x7F; 		// reading: 0..127 (7-bit)
	if(reading <= ZONE_LOW_END)		// Reading 0..60 -> Backward
	{
		speed = (unsigned int)(62 - reading) * (unsigned int)PWM_MAX / 62U;
		*dir_out = DIR_CCW;  /* choose forward/backward mapping based on wiring */
		return (unsigned char)speed;
	}
	else if(reading >= ZONE_NEUTRAL_L
		&& reading <= ZONE_NEUTRAL_H) // Reading 61..67 -> Neutral
	{
		*dir_out = DIR_OFF;
		return 0;
	}
	else 							// Reading 68..127 -> Forward
	{
		speed = (unsigned int)(reading - ZONE_HIGH_START) * (unsigned int)PWM_MAX / (127U - (unsigned int)ZONE_HIGH_START);
		*dir_out = DIR_CW;
		return (unsigned char)speed;
	}
}

/* steer_sign_out: +1 right, -1 left, 0 none */
unsigned char map_left_right(unsigned char lr_byte, signed char *steer_sign_out)
{
	unsigned char reading;
	unsigned int speed; 			// speed 0..PWM_MAX

	reading = lr_byte & 0x7F; 		// reading: 0..127 (7-bit)
	if(reading <= ZONE_LOW_END)
	{
		speed = (unsigned int)(62 - reading) * (unsigned int)PWM_MAX / 62U;
		*steer_sign_out = 1;  /* right */
		return (unsigned char)speed;
	}
	else if(reading >= ZONE_NEUTRAL_L
		&& reading <= ZONE_NEUTRAL_H) // Neutral between 61..67
	{
		*steer_sign_out = 0;
		return 0;
	}
	else 							// Reading 68..127 -> Left
	{
		speed = (unsigned int)(reading - ZONE_HIGH_START) * (unsigned int)PWM_MAX / (127U - (unsigned int)ZONE_HIGH_START);
		*steer_sign_out = -1; /* left */
		return (unsigned char)speed;
	}
}

/* ===================== Motor Control ===================== */

/* Motor1 direction: PB0, PB1 */
void Motor1_set(unsigned char dir, unsigned char speed)
{
	PORTB &= ~(0x03);
	if(dir == DIR_OFF) {}   // 00 -> do nothing
	else if(dir == DIR_CW)	// 01 -> PB0=1
		PORTB |= 0x01;
	else 				 	// 10 -> PB1=1
		PORTB |= 0x02;
	PWMDTY0 = speed; 	  	// Set the Speed for Motor-1 PWM0 -> PP0
}

/* Motor2 direction: PB2, PB3 */
void Motor2_set(unsigned char dir, unsigned char speed)
{
	PORTB &= ~(0x0C);
	if(dir == DIR_OFF) {}   // 00 -> do nothing
	else if(dir == DIR_CW) 	// 01 -> PB2=1
		PORTB |= 0x04;
	else					// 10 -> PB3=1
		PORTB |= 0x08;
	PWMDTY1 = speed;        // Set the Speed for Motor-2 PWM1 -> PP1
}

/* ===================== PWM Init ===================== */
void PWM_init_motors(void)
{
	/* Use Clock A for PWM0/PWM1 */
	PWMCLK &= ~(0x03);		// select SA for PWM0 and PWM1
	PWMPOL |=  (0x03);		// PWM0 and PWM1 active HIGH (1 polirity)
	PWMPRCLK = 0x07;      	// Prescale the E-clock by 128 --> 24M/128 = 187.5 Khz
	PWMSCLA  = 0x04;      	// Prescale A by 8 (4*2) to get SA --> 187.5KHz/8 = 23.43KHz
	PWMCTL   = 0x0C;      	// Select 8-bit mode and disable PWM in wait and freeze modes
	PWMPER0 = PWM_MAX;		// PWM_MAX = 235 for ~100% duty cycle -> // this will get us 10.02 ms period (10.02 ms =235 counts * 42.666 micro Second per count)
	PWMPER1 = PWM_MAX;		// PWM_MAX = 235 for ~100% duty cycle
	PWMDTY0 = 0;			// Motor-1 speed initial duty cycle = 0
	PWMDTY1 = 0;			// Motor-2 speed initial duty cycle = 0
	PWMCNT0 = 0;			// clear PWM0 counter
	PWMCNT1 = 0;			// clear PWM1 counter
	PWME |= 0x03; 			// enable PWM0 and PWM1
}

/* ===================== PLL ===================== */
void PLL_init(void)
{
	SYNR = 2;
	REFDV = 0;		        // at 8MHz Osc -->  48MHz PLL
	PLLCTL = 0x60;			// ON, Auto
	while(!(CRGFLG & 0x08));	// Wait the lock BIT TO SET
	CLKSEL |= 0x80;			// Select the PLL clk
}

/* ===================== SCI0 (Debug) and SCI1 (Arduino) ===================== */
void SCI0_init(void)
{
	SCI0BDL = 156;
	SCI0BDH = 0;		// 9600bps @ 24MHz E clk
	SCI0CR1 = 0;		// No Loop, 8 data bits, no parity
	SCI0CR2 = 0x0C;	// Enable Tx, Rx "Rx not used here"
}

void SCI0_Tx(unsigned char myByte)
{
	while(!(SCI0SR1 & 0x80));
	SCI0DRL = myByte;
}

// unsigned char SCI0_Rx(void){
//   while(!(SCI0SR1 & 0x20));
//   return(SCI0DRL);
// }

void PrintStringSCI0(unsigned char *msg)
{
	unsigned long i = 0;
	while(msg[i] != '.')
	{
		SCI0_Tx(msg[i]);
		i++;
	}
}
void SCI1_init(void)
{
	SCI1BDL = 156;
	SCI1BDH = 0;		// 9600bps @ 24MHz E clk
	SCI1CR1 = 0;		// No Loop, 8 data bits, no parity
	SCI1CR2 = 0x0C;	// Enable Tx, Rx "Tx not used here"
}

// void SCI1_Tx(unsigned char myByte){
//   while(!(SCI1SR1 & 0x80));
//   SCI1DRL = myByte;
// }

// From Arduino to HCS12
unsigned char SCI1_Rx(void)
{
	while(!(SCI1SR1 & 0x20));
	return SCI1DRL;
}
