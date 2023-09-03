/***************************************************************************//**
  @file     drv_ENCODER.c
  @brief    encoder_a driver. Advance implementation
  @author   Alito
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drv_ENCODER.h"
#include "board.h"
#include "drv_K64.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void detecTurn(void);
static void buttonPress(void);
static void fun(void);
static void check_expired(void);
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static uint8_t stateA;
static uint8_t	stateB;
static uint8_t lastStatusA;
static uint8_t lastStatusB;
static tim_id_t antiBouncingTimer;
static tim_id_t buttonPressTimer;
static tim_id_t antiInterrup;
static tim_id_t antiInterrupBouncing;
static uint8_t encoderInter;
static tim_id_t timer_check_expired;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/





static ENCODER_Type encoder_a;
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void drv_ENCODER_init(void) {
    gpioMode(PORT_ENCODER_RCHA, INPUT);
    gpioMode(PORT_ENCODER_RCHB, INPUT);
    gpioMode(PORT_ENCODER_BUTTON,INPUT);
    gpioIRQ(PORT_ENCODER_RCHB,PORT_eInterruptRising, encoderEvent);
    gpioIRQ(PORT_ENCODER_BUTTON,PORT_eInterruptFalling, ptrToRSwitch);
    // Inicializo la estructura del encoder_a
    encoder_a.cantGiros = 0;
    encoder_a.directRight = 0;
    encoder_a.directLeft = 0;
    encoder_a.cantPresion = 0;
    encoder_a.newState = 0;
    encoder_a.prevStateButton = 1;
    encoder_a.newStateButton = 0;
    readStatus();
    antiBouncingTimer = timerGetId();
    buttonPressTimer = timerGetId();
    antiInterrup = timerGetId();
    antiInterrupBouncing = timerGetId();

    timer_check_expired = timerGetId();

    timerStart(timer_check_expired,TIMER_MS2TICKS(10),TIM_MODE_PERIODIC,check_expired);
}

void encoderEvent(void) {
	stateA = gpioRead(PORT_ENCODER_RCHA);
	stateB = gpioRead(PORT_ENCODER_RCHB);
	if(!timerRunning(antiBouncingTimer)){
		encoder_a.newState = (stateA << 1) | stateB;  // Calcula el nuevo estado

		// Compara el nuevo estado con el estado anterior para determinar la dirección
		if (encoder_a.newState == ((encoder_a.prevState + 1) & STATE_11)) {
			encoder_a.cantGiros++;
			}
		else if (encoder_a.newState == ((encoder_a.prevState - 1) & STATE_11)) {
			encoder_a.cantGiros--;
		}
		timerStart(antiBouncingTimer,TIMER_MS2TICKS(200),TIM_MODE_SINGLESHOT,detecTurn);
		encoderInter = TURN;
	}

}
static void detecTurn(void){

/*	encoder_a.newState = ((stateA << 1) | stateB);


	CASOS
	AB
	00
	10
	01
	11

	 GirDer 00     _CERO
	 GirDer 10	   _DOS
	 GirDer 11     _TRES
	 GirDer 01    _UNO


	 GirIzq  00   _CERO
	 GirIzq  01   _UNO
	 GirIzq  11   _TRES
	 GirIzq  10   _DOS



	switch(encoder_a.newState){
		case(STATE_00):
				if(encoder_a.prevState == STATE_01){
					encoder_a.cantGiros++;
				}
				else if(encoder_a.prevState == STATE_10){
					encoder_a.cantGiros--;
				}
				break;
		case(STATE_01):
				if(encoder_a.prevState == STATE_11){
					//encoder_a.cantGiros++;
				}
				else if(encoder_a.prevState == STATE_00){
					//encoder_a.cantGiros--;
				}
				break;
		case(STATE_10):
				if(encoder_a.prevState == STATE_00){
					//encoder_a.cantGiros++;
				}
				else if(encoder_a.prevState == STATE_11){
					//encoder_a.cantGiros--;
				}
				break;
		case(STATE_11):
				if(encoder_a.prevState == STATE_10){
					encoder_a.cantGiros++;
				}
				else if(encoder_a.prevState == STATE_01){
					encoder_a.cantGiros--;
				}
				break;

	}*/


/*	encoder_a.newState = (stateA << 1) | stateB;  // Calcula el nuevo estado

	// Compara el nuevo estado con el estado anterior para determinar la dirección
	if (encoder_a.newState == ((encoder_a.prevState + 1) & STATE_11)) {
		encoder_a.cantGiros++;
		}
	else if (encoder_a.newState == ((encoder_a.prevState - 1) & STATE_11)) {
		encoder_a.cantGiros--;
	}
	encoder_a.prevState = encoder_a.newState;*/

}



void ptrToRSwitch(void){

	if(!timerRunning(antiInterrup)){
		timerStart(antiInterrup,TIMER_MS2TICKS(250),TIM_MODE_SINGLESHOT,fun);
		if(!timerRunning(buttonPressTimer)){
			encoder_a.cantPresion = 1;
			timerStart(buttonPressTimer,TIMER_MS2TICKS(1500),TIM_MODE_SINGLESHOT,buttonPress);
		}
		else if(timerRunning(buttonPressTimer)){
			encoder_a.cantPresion++;
		}
	}

}

static void buttonPress(void){
	//turnOn_DebugLed_1();
}

void readStatus(){
	lastStatusA = gpioRead(PORT_ENCODER_RCHA);
	lastStatusB = gpioRead(PORT_ENCODER_RCHB);
	encoder_a.prevState = (stateA << 1) | stateB;
}
static void fun(void){
	//turnOn_DebugLed_1();
}
uint8_t encoderInterrup (void){
	return encoderInter;
}

bool clearEncoderInter(void){
	encoderInter = false;
	return encoderInter;
}

ENCODER_Type getEncoder(void){
	return encoder_a;
}

void clearEncoderGiros (void){
	encoder_a.cantGiros = 0;
}

void clearEncoderPresiones (void){

	encoder_a.cantPresion = 0;
}

void check_expired(void){
	if(timerExpired(buttonPressTimer)){
			encoderInter = BUTTON;
		}
}


/******************************************************************************/