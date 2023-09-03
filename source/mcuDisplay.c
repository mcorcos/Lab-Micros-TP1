/*
 * mcuDisplay.c
 *
 *  Created on: 31 ago. 2023
 *      Author: Guada Voss
 */
#include "mcuDisplay.h"

#include "hardware.h"
#include "timer.h"
#include "board.h"
//#include "SysTick.h"


static display_t display;
// Macro para obtener el bit en la posición n de un número
#define GET_BIT(num, n) (((num) >> (n)) & 1)

// Para controlar la frecuencia a la que se muestra
#define TIME_DISPLAY	1 //1 ms (sisTik con 100000)
#define TIME_CHARACTER	250 //0.5s


//	TEXTO
static uint8_t arr_text[LEN_TEXT];
static int char_counter = 0;

/*********		FUNCIONES DE ACA	******************/
//	Mueve todos los caracteres un display adelante
void next_character(void);
//	Avanza al siguiente estado de display
void next_status(void);
// Imprime un display
void printMcuD(void);



/*	initMcuDisplay Inicializa todo lo necesario para usar el diplay*/
void initMcuDisplay(void){

	// Inicializo el display
	display.value_0 = 0;
	display.value_1 = 0;
	display.value_2 = 0;
	display.value_3 = 0;
	display.status = D0;
	display.text_len = 0;
	display.brighness = INIT_BRIGHNESS; //Brillo máximo
	display.mode = MOVE;
	//	Inicializo los puertos.
	gpioMode(PIN_BUS_0, OUTPUT);
	gpioMode(PIN_BUS_1, OUTPUT);
	gpioMode(PIN_BUS_2, OUTPUT);
	gpioMode(PIN_BUS_3, OUTPUT);
	gpioMode(PIN_BUS_4, OUTPUT);
	gpioMode(PIN_BUS_5, OUTPUT);
	gpioMode(PIN_BUS_6, OUTPUT);
	gpioMode(PIN_BUS_7, OUTPUT);

	gpioMode(PIN_STATUS_0, OUTPUT);
	gpioMode(PIN_STATUS_1, OUTPUT);


	tim_id_t displayTimer = timerGetId();
	timerStart(displayTimer,TIMER_MS2TICKS(TIME_DISPLAY),TIM_MODE_PERIODIC, updateMcuDisplay);

	tim_id_t nextCharacterTimer = timerGetId();
	timerStart(nextCharacterTimer,TIMER_MS2TICKS(TIME_CHARACTER),TIM_MODE_PERIODIC, next_character);
}


/*	updateMcuDisplay Se debe llamar constantemente para cambiar el display que se muestra*/
void updateMcuDisplay (void){
	int static brighness_counter = 0;
	// TIME_DISPLAY: frecuencia que cambia el display que se muestra
	if(brighness_counter == 0){
		printMcuD();
		next_status();
		brighness_counter = display.brighness;
	}
	--brighness_counter;
}


void textToDisplay(int index, uint8_t letter){
	arr_text[index]= letter;
}

void textLenDisplay(int len){
	display.text_len = len;
	char_counter = 0;
}


/********************************************************************************/
void next_character() {
	if((display.text_len < 5)){
		display.value_0 = arr_text[char_counter + 4];
		display.value_1 = arr_text[char_counter + 5];
		display.value_2 = arr_text[char_counter + 6];
		display.value_3 = arr_text[char_counter + 7];
	}
	else if((display.mode != MOVE)){
		display.value_0 = arr_text[display.mode + 0];
		display.value_1 = arr_text[display.mode + 1];
		display.value_2 = arr_text[display.mode + 2];
		display.value_3 = arr_text[display.mode + 3];
	}
	else if ( (char_counter) <= (display.text_len + 3)) {
		display.value_0 = arr_text[char_counter + 0];
		display.value_1 = arr_text[char_counter + 1];
		display.value_2 = arr_text[char_counter + 2];
		display.value_3 = arr_text[char_counter + 3];
		++char_counter;
	}
	else {
		char_counter = 0;
	}
}

/********************************************************************************/
void next_status() {

	switch (display.status) {
	case D0:
		display.status = D1;
		break;
	case D1:
		display.status = D2;
		break;
	case D2:
		display.status = D3;
		break;
	case D3:
		display.status = D0;
		break;
	default: display.status = D0;
	}
}


void printMcuD(){
	switch(display.status){

		case D0:
			gpioWrite(PIN_BUS_0, GET_BIT(display.value_0, 0));
			gpioWrite(PIN_BUS_1, GET_BIT(display.value_0, 1));
			gpioWrite(PIN_BUS_2, GET_BIT(display.value_0, 2));
			gpioWrite(PIN_BUS_3, GET_BIT(display.value_0, 3));
			gpioWrite(PIN_BUS_4, GET_BIT(display.value_0, 4));
			gpioWrite(PIN_BUS_5, GET_BIT(display.value_0, 5));
			gpioWrite(PIN_BUS_6, GET_BIT(display.value_0, 6));
			gpioWrite(PIN_BUS_7, GET_BIT(display.value_0, 7));

			break;
		case D1:
			gpioWrite(PIN_BUS_0, GET_BIT(display.value_1, 0));
			gpioWrite(PIN_BUS_1, GET_BIT(display.value_1, 1));
			gpioWrite(PIN_BUS_2, GET_BIT(display.value_1, 2));
			gpioWrite(PIN_BUS_3, GET_BIT(display.value_1, 3));
			gpioWrite(PIN_BUS_4, GET_BIT(display.value_1, 4));
			gpioWrite(PIN_BUS_5, GET_BIT(display.value_1, 5));
			gpioWrite(PIN_BUS_6, GET_BIT(display.value_1, 6));
			gpioWrite(PIN_BUS_7, GET_BIT(display.value_1, 7));

			break;
		case D2:
			gpioWrite(PIN_BUS_0, GET_BIT(display.value_2, 0));
			gpioWrite(PIN_BUS_1, GET_BIT(display.value_2, 1));
			gpioWrite(PIN_BUS_2, GET_BIT(display.value_2, 2));
			gpioWrite(PIN_BUS_3, GET_BIT(display.value_2, 3));
			gpioWrite(PIN_BUS_4, GET_BIT(display.value_2, 4));
			gpioWrite(PIN_BUS_5, GET_BIT(display.value_2, 5));
			gpioWrite(PIN_BUS_6, GET_BIT(display.value_2, 6));
			gpioWrite(PIN_BUS_7, GET_BIT(display.value_2, 7));

			break;
		case D3:
			gpioWrite(PIN_BUS_0, GET_BIT(display.value_3, 0));
			gpioWrite(PIN_BUS_1, GET_BIT(display.value_3, 1));
			gpioWrite(PIN_BUS_2, GET_BIT(display.value_3, 2));
			gpioWrite(PIN_BUS_3, GET_BIT(display.value_3, 3));
			gpioWrite(PIN_BUS_4, GET_BIT(display.value_3, 4));
			gpioWrite(PIN_BUS_5, GET_BIT(display.value_3, 5));
			gpioWrite(PIN_BUS_6, GET_BIT(display.value_3, 6));
			gpioWrite(PIN_BUS_7, GET_BIT(display.value_3, 7));

			break;
		default:
			gpioWrite(PIN_BUS_0, 0);
			gpioWrite(PIN_BUS_1, 0);
			gpioWrite(PIN_BUS_2, 0);
			gpioWrite(PIN_BUS_3, 0);
			gpioWrite(PIN_BUS_4, 0);
			gpioWrite(PIN_BUS_5, 0);
			gpioWrite(PIN_BUS_6, 0);
			gpioWrite(PIN_BUS_7, 0);

		}
	gpioWrite(PIN_STATUS_0, GET_BIT(display.status, 0));
	gpioWrite(PIN_STATUS_1, GET_BIT(display.status, 1));

}
 void modeToDisplay(int mode){
	 if( (mode>0) && (mode<=display.text_len) ){
		 display.mode = mode;
	 }
	 else{
		 display.mode = MOVE;
	 }
}

void brighnessDisplay(int value_brighness){
	display.brighness = value_brighness;
}



