/*
 * CRC.c
 *
 *  Created on: 15 Oct 2022
 *      Author: stein
 */

#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"

uint8_t crc_byte;

/**
* @brief: 	Berekent CRC van meegegeven byte
* @param: 	check_byte, byte voor CRC calc
* @return: 	CRC_Builder
*/
uint8_t CRC_Builder(char *b)
{
	uint8_t poly = 0b00000111;
	uint8_t hold =0;
	uint8_t place =0;
	int length = strlen(b);

	for(int j=8; j>0; j--,b++)
	{
		hold = *b;
		hold <<= j-1;
		place |= hold;
	}

	for(int i=length-8;i>0;i--)
	{
		b++;
		if(MSB_Check(place))
		{
			place <<= 1;
			place |= *b;
			place ^= poly;
		}
		else
		{
			place <<= 1;
			place |= *b;
		}
	}
//	UART_puts("\nCRC: ");
//	UART_putint(place);
	return place;
}

/**
* @brief: 	Checkt MSB voor een 1 of 0
* @param:	byte
* @return: 	NSB_Check
*/
int MSB_Check(uint8_t check)
{
	uint8_t op = 0x80;
	if(op & check)
		return TRUE;
	else
		return FALSE;
}
