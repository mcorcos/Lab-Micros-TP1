/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   G4
 ******************************************************************************/


#ifndef _DRV_MAGTEK_H_
#define _DRV_MAGTEK_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "drv_K64.h"
#include "board.h"
#include "debug.h"
#include "gpio.h"
#include "drv_DEVBOARD.h"

 /*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/





 /*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef void(*ptr_to_fun)(const uint8_t* data);
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/




/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
 void initMagtek();
 void ptrToClock(void);
 void ptrToEnable(void);

 typedef struct{
 	uint8_t PAN[19];
 	uint8_t ADD[18] ;
 }ID;

//
uint8_t* get_ID(void);
 //has event
bool magtek_iter(void);

bool magtek_clear(void);

#endif // _DRV_MAGTEK_H_








