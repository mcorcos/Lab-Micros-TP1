/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Grupo 4 Laboratorio de Microprocesadores:
  	  	  	Corcos, Manuel
  	  	  	Lesiuk, Alejandro
  	  	  	Paget, Milagros
  	  	  	Voss, Maria de Guadalupe
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"
#include "drv_K64.h"
#include "fsm.h"
#include "drv_ENCODER.h"
#include "drv_MAGTEK.h"
#include "timer.h"
#include "drv_DEVBOARD.h"
#include "display.h"
#include "drv_DEVLEDS.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	init_DEVLED();
	NVIC_SetPriority(PD,0);
	NVIC_SetPriority(PA,1);
	NVIC_SetPriority(PB,1);
	NVIC_SetPriority(PC,1);
	NVIC_SetPriority(PE,1);
	init_K64Leds();
	initMagtek();
	timerInit();
	drv_ENCODER_init();
	init_DEVBOARD();
	displayInit();



}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	fsm();
//	if(timerExpired(3)){
//		turnOff_DebugLed_1();
//	}
}
