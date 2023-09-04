/*
 * display.h
 *
 *  Created on: 31 ago. 2023
 *      Author: Guada Voss
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include "mcuDisplay.h"

/*******************************************************************************
  * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
  ******************************************************************************/

//	Agregar nuevo texto
void displayInit(void);
void print_msg(char* arr_new_text, int mode);
void change_brightness(int value);

#endif /* DISPLAY_H_ */
