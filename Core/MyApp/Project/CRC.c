/**
* @file 	CRC.c
* @brief 	Hier wordt de CRC berekend van een meegegeven bitarray
* 			De berekende CRC wordt dan gereturned als byte
*
* @author 	Stein van Vliet

* @date 	15/10/2022
*/

#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"

uint8_t crc_byte;

/**
* @brief: 	Berekent CRC van meegegeven bitarray
* @param: 	*b(pointer van bitarray), length(lengte van bitarray)
* @return: 	CRC_Builder
*/
uint8_t CRC_Builder(char *b, int length)
{
	uint8_t poly = 0b00000111;
	uint8_t hold =0;
	uint8_t place =0;

	//Vult een byte met de eerste 8 waarden van de array
	for(int j=8; j>0; j--,b++)
	{
		hold = *b;
		place |= hold;
		if(j>1)
			place <<=1;
	}
//	UART_putint(place);
	//Gaat CRC berekening af over de array
	for(int i=0;i<length;i++)
	{
		//Checkt of MSB 1 of 0 is, bitshift en dan wel of geen exor
		if(MSB_Check(place))
		{
			place <<= 1;
			if(length-i>8)
			{
				place |= (*b && 1);
			}
			place ^= poly;
		}
		else
		{
			place <<= 1;
			if(length-i>8)
			{
				place |= (*b && 1);
			}
		}
		if(length-i>8)
			b++;
	}

	//Debug
	if(Uart_debug_out & TEST_DEBUG_OUT)
	{
		UART_puts("\nCRC: ");
		UART_putint(place);
	}
	return place;
}

/**
* @brief: 	Checkt MSB van meegegeven byte voor een 1 of 0
* @param:	check (byte voor check)
* @return: 	MSB_Check
*/
int MSB_Check(uint8_t check)
{
	uint8_t op = 0x80;
	if(op & check)
		return TRUE;
	else
		return FALSE;
}
