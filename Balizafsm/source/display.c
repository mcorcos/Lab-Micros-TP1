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
	static int prev_brightness = 0;


	if(brighnes_value == MAX_BRIGHNESS){

		if(value<prev_brightness){
			brighnes_value = MID_BRIGHNESS;
		}

	}
	else if(brighnes_value == MID_BRIGHNESS){

		if(value>prev_brightness){
			brighnes_value = MAX_BRIGHNESS;

		}
		else if(value<prev_brightness){
			brighnes_value = MIN_BRIGHNESS;

		}

	}
	else if(brighnes_value == MIN_BRIGHNESS){

		if(value>prev_brightness){
			brighnes_value = MID_BRIGHNESS;

		}
	}

	prev_brightness = value;
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
