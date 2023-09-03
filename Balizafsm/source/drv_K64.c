/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   G4
 ******************************************************************************/


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/


#include "board.h"
#include "gpio.h"
#include <stdbool.h>

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


 void init_K64Leds(void){
	 gpioMode (PIN_LED_RED,OUTPUT);
	 gpioWrite(PIN_LED_RED,false); // prendo y apago para ver que entro
	 gpioWrite(PIN_LED_RED,true);
	 gpioMode (PIN_LED_GREEN,OUTPUT);
	 gpioWrite(PIN_LED_GREEN,false); // prendo y apago para ver que entro
	 gpioWrite(PIN_LED_GREEN,true);
	 gpioMode (PIN_LED_BLUE,OUTPUT);
	 gpioWrite(PIN_LED_BLUE,false); // prendo y apago para ver que entro
	 gpioWrite(PIN_LED_BLUE,true);


 }

 void turnOn_RedLed(void){
	 gpioWrite(PIN_LED_RED,false);
 }
 void turnOn_GreenLed(void){
	 gpioWrite(PIN_LED_GREEN,false);
 }
 void turnOn_BlueLed(void){
	 gpioWrite(PIN_LED_BLUE,false);
 }

 void turnOff_RedLed(void){
	 gpioWrite(PIN_LED_RED,true);
 }
 void turnOff_GreenLed(void){
	 gpioWrite(PIN_LED_GREEN,true);
 }
 void turnOff_BlueLed(void){
	 gpioWrite(PIN_LED_BLUE,true);
 }


 void toggle_RedLed(void){
	 gpioToggle(PIN_LED_RED);
 }

 void toggle_GreenLed(void){
	 gpioToggle(PIN_LED_GREEN);
 }

 void toggle_BlueLed(void){
	 gpioToggle(PIN_LED_BLUE);
 }






