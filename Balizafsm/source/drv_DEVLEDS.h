/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   G4
 ******************************************************************************/


#ifndef _X_H_
#define _X_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include<stdbool.h>
#include "board.h"
#include "timer.h"
#include "gpio.h"

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
void init_DEVLED(void);
void turnOn_D1Led(void);
void turnOn_D2Led(void);
void turnOn_D3Led(void);
void turnOff_D1Led(void);
void turnOff_D2Led(void);
void turnOff_D3Led(void);

#endif // _X_H_







