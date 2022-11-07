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
uint8_t CRC_Builder(char *b, int length)
{
	uint8_t poly = 0b00000111;
	uint8_t hold =0;
	uint8_t place =0;

	for(int j=8; j>0; j--,b++)
	{
		hold = *b;
//		UART_putint(*b);
		place |= hold;
		if(j>1)
			place <<=1;
	}
//	UART_puts("\r\n");
//	UART_putint(place);
//	UART_puts("\r\n");
//	UART_putint(length);
	for(int i=0;i<length;i++)
	{

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
//		UART_puts("\r\n");
//		UART_putint(place);
	}
	if(Uart_debug_out & TX_DEBUG_OUT)
	{
		UART_puts("\nCRC: ");
		UART_putint(place);
	}
	return place;
}

/**
* @brief: 	Checkt MSB voor een 1 of 0
* @param:	byte
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
