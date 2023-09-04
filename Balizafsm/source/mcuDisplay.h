/*
 * mcuDisplay.h
 *
 *  Created on: 31 ago. 2023
 *      Author: Guada Voss
 */

#ifndef MCUDISPLAY_H_
#define MCUDISPLAY_H_

#include <stdint.h>

/*************	DEFINICIONES PARA ESTRUCTURA	*************/
#define LEN_TEXT 50
enum { D0, D1, D2, D3 };

/*************		ESTRUCTURA DISPLAY		*****************/
typedef struct {
	uint8_t value_0;
	uint8_t value_1;
	uint8_t value_2;
	uint8_t value_3;
	uint8_t status;
	int text_len;
	int brighness;
	int mode;
} display_t;

#define MIN_BRIGHNESS	1
#define MID_BRIGHNESS	4
#define MAX_BRIGHNESS	8
#define INIT_BRIGHNESS	1

#define MOVE	-1

/*	1ms	no titila
 * 	4ms no titila
 *  8ms titila
 * */


/*************	FUNCIONES PARA LLAMAR AFUERA	************/
void initMcuDisplay(void);
void updateMcuDisplay (void);

void textToDisplay(int index, uint8_t letter);
void textLenDisplay(int len);
void brighnessDisplay(int value_brighness);
void modeToDisplay(int mode);




#endif /* MCUDISPLAY_H_ */
