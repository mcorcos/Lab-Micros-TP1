/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   G4
 ******************************************************************************/

#ifndef _drv_DEVBOARD_H_
#define _drv_DEVBOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "gpio.h"
#include "board.h"
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
 * function PROTOTYPES WITH local SCOPE
 ******************************************************************************/
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


void init_DEVBOARD(void);
void turnOn_ErrorLed(void);
void turnOn_DebugLed_1(void);
void turnOff_DebugLed_1(void);
void turnOn_DebugLed_2(void);
void turnOff_DebugLed_2(void);






#endif // _drv_DEVBOARD_H_





