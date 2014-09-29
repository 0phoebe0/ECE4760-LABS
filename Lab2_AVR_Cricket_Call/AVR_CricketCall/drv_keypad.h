#ifndef __DRV_KEYPAD_H__
#define __DRV_KEYPAD_H__

#include <avr/io.h>
#include <stdint.h>

#define KEYPAD_4X4		0
#define KEYPAD_4X3		1

#define KEYPAD_TYPE		KEYPAD_4X4

/*							 key line: 
	1 2 3 A - 5 PORTA.4		|	0b1110|1110: 1	0b1110|1101: 2	0b1110|1011: 3	0b1110|0111: A
	4 5 6 B - 6 PORTA.5		|	0b1101|1110: 4  0b1101|1101: 5  0b1101|1011: 6	0b1101|0111: B
	7 8 9 C - 7 PORTA.6		|	0b1011|1110: 7  0b1011|1101: 8  0b1011|1011: 9  0b1011|0111: C
	O F E D - 8 PORTA.7		|	0b0111|1110: O  0b0111|1101: F  0b0111|1011: E  0b0111|0111: D
	| | | |
	1 2 3 4					PORTA[7:4]: Input with internal resistor
	0 1 2 3 - PORTA[3:0]		PORTA[3:0]: Output
*/

typedef enum {
	KEY_SCANNING = (0),
	KEY_DEBOUNCE = (1),
	KEY_RELEASED = (2)
} keypad_scan_t;

#define KEY_RD_MSK		0xF0

#define NO_KEY			255
#define KEY_NUM_1		1
#define KEY_NUM_2		2
#define KEY_NUM_3		3
#define KEY_NUM_4		4
#define KEY_NUM_5		5
#define KEY_NUM_6		6
#define KEY_NUM_7		7
#define KEY_NUM_8		8
#define KEY_NUM_9		9
#define KEY_EXT_A		10
#define KEY_EXT_B		11
#define KEY_EXT_C		12
#define KEY_EXT_D		13
#define KEY_EXT_E		14
#define KEY_EXT_F		15
#define KEY_EXT_O		16

#define KEYPAD_PORT		PORTA
#define KEYPAD_PDDR		DDRA	
#define KEYPAD_RDPORT	PINA

void	Drv_KeyPad_Init	(void);
uint8_t Drv_KeyPadScan	(void);

#endif