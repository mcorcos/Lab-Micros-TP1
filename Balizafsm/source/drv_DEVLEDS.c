/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "drv_DEVLEDS.h"

 /*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



 /*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


void init_DEVLED(void){

	gpioMode(DEV_LED_STATUS_0,OUTPUT);
	gpioMode(DEV_LED_STATUS_1,OUTPUT);


	//Blink led 1
	turnOn_D1Led();
	turnOff_D1Led();

	//Blink led 2
	turnOn_D2Led();
	turnOff_D2Led();

	//Blink led 3
	turnOn_D3Led();
	turnOff_D3Led();



	//Blink led 1
	turnOn_D1Led();
	turnOff_D1Led();

	//Blink led 2
	turnOn_D2Led();
	turnOff_D2Led();

	//Blink led 3
	turnOn_D3Led();
	turnOff_D3Led();



}

void turnOn_D1Led(void){
	gpioWrite(DEV_LED_STATUS_0,true);
	gpioWrite(DEV_LED_STATUS_1,false);
}
void turnOn_D2Led(void){
	gpioWrite(DEV_LED_STATUS_0,false);
	gpioWrite(DEV_LED_STATUS_1,true);
}
void turnOn_D3Led(void){
	gpioWrite(DEV_LED_STATUS_0,true);
	gpioWrite(DEV_LED_STATUS_1,true);
}
void turnOff_D1Led(void){
	gpioWrite(DEV_LED_STATUS_0,false);
	gpioWrite(DEV_LED_STATUS_1,false);
}
void turnOff_D2Led(void){
	gpioWrite(DEV_LED_STATUS_0,false);
	gpioWrite(DEV_LED_STATUS_1,false);
}
void turnOff_D3Led(void){
	gpioWrite(DEV_LED_STATUS_0,false);
	gpioWrite(DEV_LED_STATUS_1,false);
}
