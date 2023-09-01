/***************************************************************************//**
  @file     drv_ENCODER.c
  @brief    Encoder driver. Advance implementation
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
static bool encoderInter;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/





static ENCODER_Type encoder;
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void drv_ENCODER_init(void) {
    gpioMode(PORT_ENCODER_RCHA, INPUT);
    gpioMode(PORT_ENCODER_RCHB, INPUT);
    gpioMode(PORT_ENCODER_BUTTON,INPUT);
    gpioMode(PORT_DEBUG,OUTPUT);
    gpioIRQ(PORT_ENCODER_RCHB,PORT_eInterruptRising, encoderEvent);
    gpioIRQ(PORT_ENCODER_BUTTON,PORT_eInterruptFalling, ptrToRSwitch);
    // Inicializo la estructura del encoder
    encoder.cantGiros = 0;
    encoder.directRight = 0;
    encoder.directLeft = 0;
    encoder.cantPresion = 0;
    encoder.newState = 0;
    encoder.prevStateButton = 1;
    encoder.newStateButton = 0;
    readStatus();
    antiBouncingTimer = timerGetId();
    buttonPressTimer = timerGetId();
    antiInterrup = timerGetId();
    antiInterrupBouncing = timerGetId();
}

void encoderEvent(void) {
	stateA = gpioRead(PORT_ENCODER_RCHA);
	stateB = gpioRead(PORT_ENCODER_RCHB);
	if(!timerRunning(antiBouncingTimer)){
		encoderInter = TURN;
		timerStart(antiBouncingTimer,TIMER_MS2TICKS(80),TIM_MODE_SINGLESHOT,detecTurn);
	}

}
static void detecTurn(void){
	encoder.newState = (stateA << 1) | stateB;  // Calcula el nuevo estado

	// Compara el nuevo estado con el estado anterior para determinar la dirección
	if (encoder.newState == ((encoder.prevState + 1) & STATE_11)) {
		encoder.cantGiros++;
		}
	else if (encoder.newState == ((encoder.prevState - 1) & STATE_11)) {
		encoder.cantGiros--;
	}

}


void ptrToRSwitch(void){

	if(!timerRunning(antiInterrup)){
		encoder.cantGiros = 0;
		encoderInter = BUTTON;
		timerStart(antiInterrup,TIMER_MS2TICKS(250),TIM_MODE_SINGLESHOT,fun);
		if(!timerRunning(buttonPressTimer)){
			encoder.cantPresion = 1;
			timerStart(buttonPressTimer,TIMER_MS2TICKS(1500),TIM_MODE_SINGLESHOT,buttonPress);
		}
		else if(timerRunning(buttonPressTimer)){
			encoder.cantPresion++;
		}

	}
}

static void buttonPress(void){

}

void readStatus(){
	lastStatusA = gpioRead(PORT_ENCODER_RCHA);
	lastStatusB = gpioRead(PORT_ENCODER_RCHB);
	encoder.prevState = (stateA << 1) | stateB;
}
static void fun(void){

}
uint8_t encoderInterrup (void){
	return encoderInter;
}

bool clearEncoderInter(void){
	encoderInter = false;
	return encoderInter;
}

ENCODER_Type* getEncoder(void){
	return &encoder;
}



/******************************************************************************/
