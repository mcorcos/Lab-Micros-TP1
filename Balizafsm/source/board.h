/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"




/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// PIN_MAGTEK_VCC CONECTAR MANUALMENTE
#define PIN_MAGTEK_ENABLE (PORTNUM2PIN(PD,2)) //AMARILLO grus
#define PIN_MAGTEK_CLOCK (PORTNUM2PIN(PD,3)) // verde blanco
#define PIN_MAGTEK_DATA (PORTNUM2PIN(PD,1)) //azul naranja
// PIN_MAGTEK_GND CONECTAR MANUALMENTE


// On Board User LEDs
#define PIN_LED_RED     PORTNUM2PIN(PB,22)
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26)
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21) // PTB21


//PINES PARA USAR
#define PIN_PTC_10		PORTNUM2PIN(PC,10)
#define PIN_PTB_11		PORTNUM2PIN(PB,11)
#define PIN_PTB_24		PORTNUM2PIN(PB,24)
#define PIN_PTB_25		PORTNUM2PIN(PB,25)
#define PIN_PTE_24		PORTNUM2PIN(PE,24)

// On Board User Switches
#define PIN_SW2         PORTNUM2PIN(PC,6)
#define PIN_SW3         PORTNUM2PIN(PA,4)

#define LED_ACTIVE      LOW
#define SW_ACTIVE       LOW			//Buscamos en el schematic y como el boton esta conectado a masa es active low
#define SW_INPUT_TYPE   // ???

//DEBUG
#define DEBUG_PIN_1 PIN_PTC_10
#define DEBUG_PIN_2 PIN_PTB_11
#define ERROR_PIN_1	PIN_PTB_25
#define TIME_PIN PIN_PTE_24



//Defines ENCODER







//Define DEVBOARD


// PIN BUS
#define PIN_BUS_0		PORTNUM2PIN(PB,2)
#define PIN_BUS_1		PORTNUM2PIN(PC,3)
#define PIN_BUS_2		PORTNUM2PIN(PC,2)
#define PIN_BUS_3		PORTNUM2PIN(PA,2)
#define PIN_BUS_4		PORTNUM2PIN(PB,23)
#define PIN_BUS_5		PORTNUM2PIN(PA,1)
#define PIN_BUS_6		PORTNUM2PIN(PB,9)
#define PIN_BUS_7		PORTNUM2PIN(PC,17)


// PIN STATE
#define PIN_STATUS_0		PORTNUM2PIN(PB,20)
#define PIN_STATUS_1		PORTNUM2PIN(PC,5)


#define DEV_LED_STATUS_0 PORTNUM2PIN(PC,8)
#define DEV_LED_STATUS_1 PORTNUM2PIN(PC,1)
// los de

#define PORT_ENCODER_RCHA (PORTNUM2PIN(PC,7))
#define	PORT_ENCODER_RCHB (PORTNUM2PIN(PC,0))
#define PORT_ENCODER_BUTTON (PORTNUM2PIN(PC,9))



/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
