/***************************************************************************//**
  @file     drv_ENCODER.h
  @brief    Simple GPIO Pin services, similar to Arduino
  @author   Alito
 ******************************************************************************/

#ifndef _DRV_ENCODER_H_
#define _DRV_ENCODER_H_



/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "MK64F12.h"
#include "hardware.h"
#include "gpio.h"
#include "timer.h"






/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
// Convert port and number into pin ID
// Ex: PTB5  -> PORTNUM2PIN(PB,5)  -> 0x25
//     PTC22 -> PORTNUM2PIN(PC,22) -> 0x56
#define PORTNUM2PIN(p,n)    (((p)<<5) + (n))
#define PIN2PORT(p)         (((p)>>5) & 0x07)
#define PIN2NUM(p)          ((p) & 0x1F)



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum EncoderState {
    STATE_00,
    STATE_01,
    STATE_10,
    STATE_11
};
enum Encoder{
	TURN = 1,
	BUTTON
};

typedef struct{
	int8_t cantGiros;
	uint8_t directRight;
	uint8_t directLeft;
	uint8_t cantPresion;
	uint8_t newState;
	uint8_t prevState;
	uint8_t newStateButton;
	uint8_t prevStateButton;
}ENCODER_Type;
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void drv_ENCODER_init(void);
void readStatus(void);
void encoderEvent (void);
void ptrToRSwitch(void);
uint8_t encoderInterrup (void);
bool clearEncoderInter (void);
void clearEncoderGiros (void);
void clearEncoderPresiones (void);
ENCODER_Type getEncoder(void);


#endif // _DRV_ENCODER_H_
