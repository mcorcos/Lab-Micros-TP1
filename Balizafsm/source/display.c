/*
 * display.c
 *
 *  Created on: 31 ago. 2023
 *      Author: Guada Voss
 */


#include "display.h"
#include "alphabet.h"


// Declaración de funciones locales
int arr_len(char* arr);
void clear_text(int len);


// FUNCIONES

void displayInit(){
	// Inicializo Display en MCU
	initMcuDisplay();
}

void change_brightness(int value){
	static int brighnes_value = INIT_BRIGHNESS ;
	int new_brighnes_value = 0;

	new_brighnes_value = brighnes_value + value;

	if(new_brighnes_value < MIN_BRIGHNESS){
		brighnes_value = MIN_BRIGHNESS; //MÁX
	}
	else if(new_brighnes_value> MAX_BRIGHNESS){
		brighnes_value = MAX_BRIGHNESS;
	}
	else{
		brighnes_value = new_brighnes_value;
	}
	brighnessDisplay(brighnes_value);

}

/********************************************************************************/

void print_msg(char* arr_new_text, int mode) {

	int counter = 0;
	static int len_arr = 0;
	clear_text(LEN_TEXT);
	len_arr = arr_len(arr_new_text);
	textLenDisplay(len_arr);
	modeToDisplay(mode);

	while (counter < len_arr) {
		textToDisplay(counter +4, translateChar(arr_new_text[counter]));
		++counter;
	}
}



/*******************	FUNCIONES PARA CHAR		**************/
int arr_len(char* arr) {
	int len = 0;
	int i = 0;

	while (arr[i] != '\0') {
		i++;
		len++;
	}
	return len;
}

void clear_text(int len) {

	int i = 0;

	while (i < len) {
		textToDisplay(i,0);
		++i;
	}

}

/********************************************************************************/
