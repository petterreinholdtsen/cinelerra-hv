#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

#include "parapin.h"

// Delay to allow registers to take up values
#define DELAY usleep(1000);
#define OFF 0



#define STOP 0
#define FORWARD 1
#define BACKWARD 2

// Mask sent to parallel port to control motors;
static unsigned int motor_mask = 0;
static unsigned int power_mask = 0;
static unsigned int switch_mask = 0;
static int analog1 = 0;
static int analog2 = 0;
static int analog3 = 0;

// Motor pin mask values
#define SLIDER_FWD LP_PIN07
#define SLIDER_BWD LP_PIN06
#define SLIDER 0

#define GRAB_FWD LP_PIN08
#define GRAB_BWD LP_PIN09
#define GRABBER 1

#define CRANE1_FWD LP_PIN03
#define CRANE1_BWD LP_PIN05
#define CRANE1 2

#define CRANE2_FWD LP_PIN04
#define CRANE2_BWD LP_PIN02
#define CRANE2 3

#define RIGHT_VERTICAL LP_PIN05
#define FRONT_SWITCH LP_PIN08
#define BACK_SWITCH LP_PIN09

#define MOTOR_TO_CHAR(mask) \
((motor_mask & (mask)) ? '*' : ' ')

#define NOTMOTOR_TO_CHAR(mask) \
(!(motor_mask & (mask)) ? '*' : ' ')

#define SWITCH_TO_CHAR(mask) \
((switch_mask & (mask)) ? '*' : '0')

static unsigned char flip_bits[256];

static void print_bin(unsigned int value)
{
	int i;
	for(i = 31; i >= 0; i--)
	{
		int result = (2 << i) & value;
		printf("%d", result ? 1 : 0);
	}
}

static void set_multiplexer(int value)
{
	if(value & 0x4)
		set_pin(LP_PIN17);
	else
		clear_pin(LP_PIN17);

	if(value & 0x2)
		set_pin(LP_PIN16);
	else
		clear_pin(LP_PIN16);

	if(value & 0x1)
		set_pin(LP_PIN14);
	else
		clear_pin(LP_PIN14);
}

static void enable_ad0()
{
	set_multiplexer(0x0);
}

static void enable_ad1()
{
	set_multiplexer(0x1);
}

static void enable_ad2()
{
	set_multiplexer(0x2);
}

static void stop_motors()
{
	set_multiplexer(0x4);
	pin_output_mode(LP_DATA_PINS);
	clear_pin(LP_DATA_PINS);
	motor_mask = 0;
	DELAY
}

static void power_on()
{
	power_mask = 1;
	clear_pin(LP_PIN01);
// Keep motors from shorting out
	stop_motors();
}

static void power_off()
{
	power_mask = 0;
	set_pin(LP_PIN01);
}

static void reset_converters()
{
	set_pin(LP_PIN17);
	clear_pin(LP_PIN17);
}

static void get_switches()
{
	set_multiplexer(0x7);
	pin_input_mode(LP_DATA_PINS);
	DELAY
	switch_mask = ~pin_is_set(LP_DATA_PINS);
}

static void print_switch_title()
{
	printf("Switch:   0 1 2 right 4 5 front back\n"
		"---------------------------------\n");
}

static void print_switches()
{
		printf("          %c %c %c %c     %c %c %c     %c\r",
		SWITCH_TO_CHAR(LP_PIN02),
		SWITCH_TO_CHAR(LP_PIN03),
		SWITCH_TO_CHAR(LP_PIN04),
		SWITCH_TO_CHAR(RIGHT_VERTICAL),
		SWITCH_TO_CHAR(LP_PIN06),
		SWITCH_TO_CHAR(LP_PIN07),
		SWITCH_TO_CHAR(FRONT_SWITCH),
		SWITCH_TO_CHAR(BACK_SWITCH));
}

static void poll_switches()
{
	print_switch_title();
	while(1)
	{
		get_switches();
		print_switches();
		fflush(stdout);
	}
}

static void robot_loop()
{
	int i;
// 00000001
	for(i = 0; i < 256; i++)
	{
		flip_bits[i] = 
			((i & 0x01) << 7) |
			((i & 0x02) << 5) |
			((i & 0x04) << 3) |
			((i & 0x08) << 1) |
			((i & 0x10) >> 1) |
			((i & 0x20) >> 3) |
			((i & 0x40) >> 5) |
			((i & 0x80) >> 7);
	}


	power_on();

	set_multiplexer(0x0);
	pin_input_mode(LP_DATA_PINS);

#if 0
	while(1)
	{
		reset_converters();
		enable_ad0();
		unsigned int ad0 = pin_is_set(LP_DATA_PINS);
		enable_ad1();
		unsigned int ad1 = pin_is_set(LP_DATA_PINS);
		enable_ad2();
		unsigned int ad2 = pin_is_set(LP_DATA_PINS);

		ad0 = flip_bits[(ad0) & 0xff];
		ad1 = flip_bits[(ad1) & 0xff];
		int diff = ad0 - ad1;
		printf("AD0=%03d AD1=%03d AD2=%03d DIFF=%03d", 
			ad0,
			ad1,
			ad2,
			diff);
/*
 * 		for(i = -20; i < diff; i++)
 * 		{
 * 			printf(" ");
 * 		}
 * 		printf("*");
 */
		printf("\r");
		fflush(stdout);
	}
#endif
}

static void spin_motor(int number, int direction)
{
	switch(number)
	{
		case SLIDER:
			motor_mask &= ~(SLIDER_FWD | SLIDER_BWD);
			if(direction == FORWARD)
				motor_mask |= SLIDER_FWD;
			else
			if(direction == BACKWARD)
				motor_mask |= SLIDER_BWD;
			break;

		case GRABBER:
			motor_mask &= ~(GRAB_FWD | GRAB_BWD);
			if(direction == FORWARD)
				motor_mask |= GRAB_FWD;
			else
			if(direction == BACKWARD)
				motor_mask |= GRAB_BWD;
			break;

		case CRANE1:
			motor_mask &= ~(CRANE1_FWD | CRANE1_BWD);
			if(direction == FORWARD)
				motor_mask |= CRANE1_FWD;
			else
			if(direction == BACKWARD)
				motor_mask |= CRANE1_BWD;
			break;

			case CRANE2:
			motor_mask &= ~(CRANE2_FWD | CRANE2_BWD);
			if(direction == FORWARD)
				motor_mask |= CRANE2_FWD;
			else
			if(direction == BACKWARD)
				motor_mask |= CRANE2_BWD;
			break;
	}
	pin_output_mode(LP_DATA_PINS);
	clear_pin((~motor_mask) & LP_DATA_PINS);
	set_pin(motor_mask & LP_DATA_PINS);
	set_multiplexer(0x4);
	DELAY
}

static unsigned int get_ad_value(int multiplex)
{
	pin_input_mode(LP_DATA_PINS);
	set_multiplexer(multiplex);
	DELAY
	unsigned char result = pin_is_set(LP_DATA_PINS);
	return result;
}

#define ACCUM_LEN 16

static void ad_value_loop()
{
	unsigned int accum1[ACCUM_LEN], accum2[ACCUM_LEN], accum3[ACCUM_LEN];
	int i = 0, j;
	while(1)
	{
// Trigger converters
		set_multiplexer(3);
		DELAY

		unsigned int result1 = get_ad_value(0);
		unsigned int result2 = get_ad_value(1);
		unsigned int result3 = get_ad_value(2);
		accum1[i] = result1;
		accum2[i] = result2;
		accum3[i] = result3;
		i++;
		i = i % ACCUM_LEN;
		result1 = 0;
		result2 = 0;
		result3 = 0;
		for(j = 0; j < ACCUM_LEN; j++)
		{
			result1 += accum1[j];
			result2 += accum2[j];
			result3 += accum3[j];
		}
		result1 /= ACCUM_LEN;
		result2 /= ACCUM_LEN;
		result3 /= ACCUM_LEN;
printf("ad0 = %03d ad1 = %03d ad2 = %03d ad2 - ad1 = %d         \r", 
result1, 
result2, 
result3,
result3 - result2);
fflush(stdout);
//break;
/*
 * 		printf("ad1=");
 * 		print_bin(result1);
 * 		printf(" %d\nad2=", result1);
 * 		print_bin(result2);
 * 		printf(" %d\nad3=", result2);
 * 		print_bin(result3);
 * 		printf(" %d\n", result3);
 */
	}
	printf("\n");
}


static void slide_forward_stop()
{
	int done = 0;
	spin_motor(SLIDER, FORWARD);
	do
	{
		get_switches();
		if(switch_mask & FRONT_SWITCH) done = 1;
	}while(!done);
	spin_motor(SLIDER, STOP);
}

static void slide_backward_stop()
{
	int done = 0;
	spin_motor(SLIDER, BACKWARD);
	do
	{
		get_switches();
		if(switch_mask & BACK_SWITCH) done = 1;
	}while(!done);
	spin_motor(SLIDER, STOP);
}

static void pull_cd_out()
{
	spin_motor(GRABBER, BACKWARD);
	sleep(1);
	slide_forward_stop();
	spin_motor(GRABBER, FORWARD);
	sleep(1);
	spin_motor(GRABBER, STOP);
	slide_backward_stop();
}

static void push_cd_in()
{
	spin_motor(GRABBER, BACKWARD);
	sleep(1);
	slide_forward_stop();
	slide_backward_stop();
	sleep(1);
	spin_motor(GRABBER, STOP);
}

static void move_crane(int distance, int direction)
{
// Move finite distance and stop
	if(distance)
	{
		int rising = 0, falling = 0, current = 0;
		get_switches();
		current = (switch_mask & (RIGHT_VERTICAL));

		spin_motor(CRANE1, direction);
		spin_motor(CRANE2, direction);
		while(1)
		{
			get_switches();
			int value = switch_mask & (RIGHT_VERTICAL);
			if(value && !current)
			{
				current = 1;
				rising++;
			}
			else
			if(!value && current)
			{
				current = 0;
				falling++;
			}
			if(rising >= distance)
				break;
		}
		stop_motors();
	}
	else
// Move indefinitely
	{
		spin_motor(CRANE1, direction);
		spin_motor(CRANE2, direction);
	}
}

static void crane_up()
{
	int distance = 0;
	printf("Enter distance or 0 for indefinite: ");
	fflush(stdout);
	scanf("%d", &distance);
	
	move_crane(distance, FORWARD);
}

static void crane_down()
{
	int distance = 0;
	printf("Enter distance or 0 for indefinite: ");
	fflush(stdout);
	scanf("%d", &distance);
	
	move_crane(distance, BACKWARD);
}

static void print_menu()
{
	printf(
"----------------------------------------------------------------------------\n"
"Heroine 1170 robot controller\n"
"Motor:     slide  grab   crane1  crane2\n"
"-------------------------------------\n"
"Forward:   q %c    w %c    e %c     r %c\n"
"Backward:  a %c    s %c    d %c     f %c\n"
"Stop:      z %c    x %c    c %c     v %c\n"
"\n"
"1 - power on %c\n"
"2 - power off %c\n"
"3 - stop all motors\n"
"4 - poll switches\n"
"5 - slide forward and stop\n"
"6 - slide backward and stop\n"
"7 - pull CD out\n"
"8 - push CD in\n"
"9 - crane up\n"
"0 - crane down\n"
"\n",
MOTOR_TO_CHAR(SLIDER_FWD),
MOTOR_TO_CHAR(GRAB_FWD),
MOTOR_TO_CHAR(CRANE1_FWD),
MOTOR_TO_CHAR(CRANE2_FWD),
MOTOR_TO_CHAR(SLIDER_BWD),
MOTOR_TO_CHAR(GRAB_BWD),
MOTOR_TO_CHAR(CRANE1_BWD),
MOTOR_TO_CHAR(CRANE2_BWD),
NOTMOTOR_TO_CHAR(SLIDER_FWD | SLIDER_BWD),
NOTMOTOR_TO_CHAR(GRAB_FWD | GRAB_BWD),
NOTMOTOR_TO_CHAR(CRANE1_FWD | CRANE1_BWD),
NOTMOTOR_TO_CHAR(CRANE2_FWD | CRANE2_BWD),
power_mask ? '*' : ' ',
power_mask ? ' ' : '*'
	);
	print_switch_title();
	print_switches();
	printf("\nCommand: ");
}


int main(int argc, char *argv[])
{
	int i;

// Initialize parallel port
  	if(pin_init_user(LPT1) < 0)
	{
		fprintf(stderr, "Failed to initialize parallel port\n");
		exit(1);
	}
	pin_input_mode(LP_DATA_PINS);
	pin_output_mode(LP_SWITCHABLE_PINS);

	power_off();
	stop_motors();

	while(1)
	{
		print_menu();
		int command = 0;
		char string[1024];

		fscanf(stdin, "%s", string);
		command = string[0];

		switch(command)
		{
			case '1': power_on();     break;
			case '2': power_off();    break;
			case '3': stop_motors();  break;
			case '4': poll_switches(); break;
			case '5': slide_forward_stop(); break;
			case '6': slide_backward_stop(); break;
			case '7': pull_cd_out(); break;
			case '8': push_cd_in(); break;
			case '9': crane_up(); break;
			case '0': crane_down(); break;
			case 'q': spin_motor(SLIDER, FORWARD);  break;
			case 'a': spin_motor(SLIDER, BACKWARD); break;
			case 'z': spin_motor(SLIDER, STOP);     break;
			case 'w': spin_motor(GRABBER, FORWARD);  break;
			case 's': spin_motor(GRABBER, BACKWARD); break;
			case 'x': spin_motor(GRABBER, STOP);     break;
			case 'e': spin_motor(CRANE1, FORWARD);  break;
			case 'd': spin_motor(CRANE1, BACKWARD); break;
			case 'c': spin_motor(CRANE1, STOP);     break;
			case 'r': spin_motor(CRANE2, FORWARD);  break;
			case 'f': spin_motor(CRANE2, BACKWARD); break;
			case 'v': spin_motor(CRANE2, STOP);     break;
		}
	}

	return 0;
}
