#include "drv_keypad.h"
#include "drv_debug.h"

const uint8_t KeyPad_ASCII_Table[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7', 
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

void Drv_KeyPad_Init(void) {
	
	/*!> Keypad Hardware Interface Initialize */
	KEYPAD_PORT = 0xFF;
			
	/*!> PORTn.0~PORTn.3: Output, PORTn.4~PORTn.7: Input with pull-up resistor */
	KEYPAD_PDDR = 0x0F;		
	
	/*!> Pay attention to the internal pull-up resistor! */
}

uint8_t Drv_KeyPadScan(void) {
	
	static keypad_scan_t key_scan_status = KEY_RELEASED;
	static uint8_t key_value;
	static uint8_t key_line;
	
	uint8_t scan_index;
	uint8_t key_return = NO_KEY;
	
	switch (key_scan_status) {
		
		case KEY_SCANNING:
			key_line = 0b00000001;
			for (scan_index = 0; scan_index < 4; scan_index++)
			{
				KEYPAD_PORT = ~key_line;
				KEYPAD_PORT = ~key_line;
				key_value = KEY_RD_MSK & KEYPAD_RDPORT;
				if (key_value == KEY_RD_MSK)				/*!> No key is pressed					*/
					key_line <<= 1;							/*!> Change line value					*/
				else {
					key_scan_status = KEY_DEBOUNCE;			/*!> Some key maybe pressed, confirm!	*/
					break;									/*!> Break scanning loop				*/
				}
			}
		break;
		
		case KEY_DEBOUNCE:
			if (key_value == (KEY_RD_MSK & KEYPAD_RDPORT))/*!> Read KeyPad Again, astill pressed!	*/
			{
//				Drv_Debug_Printf("%x!\r\n", ((key_line) | key_value));
				switch(key_line | key_value) {
					case 0b11100001:		key_return = KEY_NUM_1;		break;
					case 0b11100010:		key_return = KEY_NUM_2;		break;
					case 0b11100100:		key_return = KEY_NUM_3;		break;
					case 0b11101000:		key_return = KEY_EXT_A;		break;
					
					case 0b11010001:		key_return = KEY_NUM_4;		break;
					case 0b11010010:		key_return = KEY_NUM_5;		break;
					case 0b11010100:		key_return = KEY_NUM_6;		break;
					case 0b11011000:		key_return = KEY_EXT_B;		break;
					
					case 0b10110001:		key_return = KEY_NUM_7;		break;
					case 0b10110010:		key_return = KEY_NUM_8;		break;
					case 0b10110100:		key_return = KEY_NUM_9;		break;
					case 0b10111000:		key_return = KEY_EXT_C;		break;
					
					case 0b01110001:		key_return = KEY_NUM_0;		break;
					case 0b01110010:		key_return = KEY_EXT_F;		break;
					case 0b01110100:		key_return = KEY_EXT_E;		break;
					case 0b01111000:		key_return = KEY_EXT_D;		break;
					
					default:
						/*!> Keypad exception */
					break;
				}
				key_scan_status = KEY_RELEASED;
			}
			else                                    /* This is just a key glitch, ignore! */
				key_scan_status = KEY_SCANNING;
		break;
		
		case KEY_RELEASED:
			KEYPAD_PORT = 0xF0;
			KEYPAD_PORT = 0xF0;
			if ((KEY_RD_MSK & KEYPAD_RDPORT) == KEY_RD_MSK)
				key_scan_status = KEY_SCANNING;
		break;
		
	}
	return key_return;
}
