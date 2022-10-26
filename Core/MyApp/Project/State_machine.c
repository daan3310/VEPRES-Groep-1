/*
 * State_machine.c
 *
 *  Created on: 16 Oct 2022
 *      Author: stein
 */

#include "main.h"
#include "cmsis_os.h"
//#include "usb_host.h"
#include "my_app.h"


/**
* @brief: 	Context switch naar juiste state
* @param: 	state, welke state naar geswitcht wordt
* @return: 	void
*/
void State_switch(int state)
{
	switch(state)
	{
	case 0:
		//eventgroup state 0
		break;
	case 1:
		//eventgroup state 1
		break;
	case 2:
		//evengroup state 2
		break;
	}
}
