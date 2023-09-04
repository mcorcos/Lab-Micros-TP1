/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   G4
 ******************************************************************************/



/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "drv_DEVBOARD.h"

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
 * function PROTOTYPES WITH local SCOPE
 ******************************************************************************/
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void init_DEVBOARD(void){

	//PINES DE ERROR

	//gpioMode(ERROR_PIN_1,OUTPUT);
	//gpioWrite(ERROR_PIN_1,false);


	//PINES DE DEBUG

	gpioMode(DEBUG_PIN_1,OUTPUT);
	gpioWrite(DEBUG_PIN_1,false);

	gpioMode(DEBUG_PIN_2,OUTPUT);
	gpioWrite(DEBUG_PIN_2,false);

	gpioMode(TIME_PIN,OUTPUT);
	gpioWrite(TIME_PIN,false);


}

//primer pin de error

 void turnOn_ErrorLed_1(void){
	 gpioWrite(ERROR_PIN_1,true);
 }

 void turnOff_ErrorLed_1(void){
	 gpioWrite(ERROR_PIN_1,false);
 }

//Primer pin de debug

 void turnOn_DebugLed_1(void){
	 gpioWrite(DEBUG_PIN_1,true);
 }

 void turnOff_DebugLed_1(void){
	 gpioWrite(DEBUG_PIN_1,false);
 }


 //segundo debug de pin

 void turnOn_DebugLed_2(void){
	 gpioWrite(DEBUG_PIN_2,true);
 }

 void turnOff_DebugLed_2(void){
	 gpioWrite(DEBUG_PIN_2,false);
 }

//time pin

 void turnOn_TimePin(void){
	 gpioWrite(TIME_PIN,true);
 }

 void turnOff_TimePin(void){
	 gpioWrite(TIME_PIN,false);
 }
