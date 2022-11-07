/**
* @file 		State_machine.c
* @brief 		Ongebruikte functie, zou dienen als selectieboom voor de verschillende states van de ontvanger
* 				Functie voornamelijk overgenomen door Sample.c en data_rx.c
*
* @author 		Stein van Vliet
* @date 		16/10/2022
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
