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
uint8_t CRC_Builder(uint8_t check_byte)
{
	uint8_t poly = 0b00000111;

//	UART_puts("\n");
//	UART_putint(poly);

	for(int i=8;i>0;i--)
	{

//		UART_puts("\n");
//		UART_putint(check_byte);

		if(MSB_Check(check_byte))
		{
			check_byte <<= 1;
			check_byte ^= poly;
		}
		else
			check_byte <<= 1;
	}

//	UART_puts("\n");

	return check_byte;
}

/**
* @brief: 	Checkt MSB voor een 1 of 0
* @param:	byte
* @return: 	NSB_Check
*/
int MSB_Check(uint8_t byte)
{
	uint8_t op = 0x80;
	op &= byte;
	op >>= 7;
	if(op)
		return TRUE;
	else
		return FALSE;
}
