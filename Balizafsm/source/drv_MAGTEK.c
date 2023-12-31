/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   G4
 ******************************************************************************/



/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include "drv_MAGTEK.h"
#include "timer.h"
#include "drv_DEVLEDS.h"




 /*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define CHAR_LENGHT (5)
#define MAX_CHARS (200)
#define MAX_PAN (19)

#define CHAR_0 0b10000
#define CHAR_1 0b00001
#define CHAR_2 0b00010
#define CHAR_3 0b10011
#define CHAR_4 0b00100
#define CHAR_5 0b10101
#define CHAR_6 0b10110
#define CHAR_7 0b00111
#define CHAR_8 0b01000
#define CHAR_9 0b11001
#define PUNTOPUNTO 0b11010
#define PUNTOCOMA 0b01011 // SS
#define MINOR 0b11100
#define EQUAL 0b01101 // FS
#define MAJOR 0b01110
#define QUESTION 0b11111 // ES

#define MIRRORED_PUNTOCOMA 0b11010


#define SIZE(dat)  (sizeof(dat)/ sizeof(dat[0]))

 /*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum { _WAITING , _READING , _END};
enum{_SS , _PAN ,_ES,_ERR,_FS};
typedef struct{
	uint8_t data_m :5;
	uint8_t nc   :3;
}character;


typedef uint32_t id_number;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


//flags
static uint8_t state_m = _WAITING;
static uint8_t status_m = _SS;

//data_m
static uint8_t data_m[MAX_CHARS];

//contador
static uint16_t bit_counter = 0;
static uint16_t shift_counter = 0;
static uint16_t iterator = 0;
static uint16_t writer = 0;

//mis tipos de datos
static character mydata;
static ID myID = {.PAN = {'0' ,'0','0','0' , '0' ,'0','0','0' , '0' ,'0','0','0' , '0' ,'0','0','0' ,'0' , '0' ,'0'}, .ADD = {'0','0','0' , '0' ,'0','0','0' , '0' ,'0','0','0' , '0' ,'0','0','0' ,'0' , '0' ,'0'} };
static id_number my_id_number;

//event
static bool magtek_interrupt = false;
static bool magtek_card_ready = false;


//timer
static tim_id_t timer_check_expired_magtek;

/*******************************************************************************
 * VARIABLE FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void flush(void);
void parse(void);
void parse_alphanumeric(character data_m);
void parse_PAN2NUM(void);
void check_card_ready(void);
void write(char character , uint8_t state_m, uint8_t iterator);
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

  void initMagtek(){

	  //Pines necesarios


	  gpioMode(PIN_MAGTEK_ENABLE,INPUT);
	  gpioMode(PIN_MAGTEK_CLOCK,INPUT);
	  gpioMode(PIN_MAGTEK_DATA,INPUT);


	  gpioIRQ(PIN_MAGTEK_ENABLE,PORT_eInterruptEither,ptrToEnable);
	  gpioIRQ(PIN_MAGTEK_CLOCK,PORT_eInterruptFalling,ptrToClock); //Por el moemnto solo necesito la interrupcion de clock

	  timer_check_expired_magtek = timerGetId();

	  timerStart(timer_check_expired_magtek,TIMER_MS2TICKS(500),TIM_MODE_PERIODIC,check_card_ready);




  }

void ptrToClock(void){
	turnOn_TimePin();
	if((state_m == _READING )){
	bool new_data = !(gpioRead(PIN_MAGTEK_DATA));
	data_m[bit_counter++] = new_data;
	}
	turnOff_TimePin();
}
 //ISR CUANDO EL ENABLE CAMBIE DE ESTADO

 void ptrToEnable(void){

	 turnOn_TimePin();
	 turnOff_RedLed();
	 if((state_m == _WAITING)){

		state_m =_READING;
	 }
	 else if ((state_m == _READING ) ) {

		 state_m = _END;
	 }
	 if(state_m == _END){
		 //interpretar
		 parse();

		 //prendo el flag!!!1



		 //ctadores en 0
		 bit_counter=0;
		 iterator =0;
		 writer =0;

		 //original state_ms
		 status_m = _SS;
		 state_m = _WAITING;
		 magtek_card_ready = true;


	 }
	 turnOff_TimePin();
 }


 void check_card_ready(void){
	 if(magtek_card_ready == true){

		 turnOn_D3Led();
		 magtek_interrupt = true;
		 magtek_card_ready = false;

	 }
 }




 void flush(void){
	 for(uint8_t i=0;i<SIZE(data_m);i++){
		 data_m[i] = 0;
	 }
	 for(uint8_t i=0;i<SIZE(myID.PAN);i++){
		 myID.PAN[i] = '0';
	 }
	 for(uint8_t i=0;i<SIZE(myID.PAN);i++){
		 myID.ADD[i] = '0';
	 }
 }

 void parse(void){

	 for(iterator=0; iterator<SIZE(data_m) && (status_m == _SS) ;iterator++){
		 mydata.data_m = (mydata.data_m<<1) | (data_m[iterator]);
		 if(mydata.data_m == MIRRORED_PUNTOCOMA){
			 status_m = _PAN;
		 }
	 }
	 if(status_m == _PAN){

		 mydata.data_m = 0b00000; //aranco con una palabra en 0
		 	 	 	 	 	 	 // El estado tiene que ser PAN , o FS      y el iterador tampoco se puede pasar de data_m
		 for(shift_counter = 0; ( (status_m == _PAN) || (status_m == _FS)  )   && (iterator < MAX_CHARS)  ;       shift_counter++,iterator++){  //para cada character itero para formar la palabra

			 mydata.data_m = mydata.data_m | (data_m[iterator]<<shift_counter); // voy creando 0b00001 la palabra
			 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 //                    ^-----
			 if(shift_counter == (CHAR_LENGHT-1)){ // cuando el contador llega a la posicion 4 , pregunto que es la palabra
				 parse_alphanumeric(mydata);
				 mydata.data_m = 0b00000;
				 shift_counter = -1; //por el post incremento del for cuando vuelve me lo deja en 0
			 }
		 }
	}
	 if(status_m == _ES){ //tengo que conseguir el lrc
		 status_m = _SS;
	 }
	 if(status_m == _ERR){


#if ERROR_MODE && ERROR_CARD
		 turnOn_ErrorLed_1();
#endif
		 status_m = _SS;
	 }
}

void parse_alphanumeric(character data_m){
	switch(data_m.data_m){
		case CHAR_0:
			write('0',status_m,writer);
			break;
		case CHAR_1:
			write('1',status_m,writer);
			break;
		case CHAR_2:
			write('2',status_m,writer);
			break;
		case CHAR_3:
			write('3',status_m,writer);
			break;
		case CHAR_4:
			write('4',status_m,writer);
			break;
		case CHAR_5:
			write('5',status_m,writer);
			break;
		case CHAR_6:
			write('6',status_m,writer);
			break;
		case CHAR_7:
			write('7',status_m,writer);
			break;
		case CHAR_8:
			write('8',status_m,writer);
			break;
		case CHAR_9:
			write('9',status_m,writer);
			break;
		case PUNTOPUNTO:
			status_m = _ERR; //en los _ERR , no tendria que reibir nada de esto
			break;
		case PUNTOCOMA:
			status_m = _ERR;
			break;
		case MINOR:
			status_m = _ERR;
			break;
		case MAJOR:
			status_m = _ERR;
			break;
		case EQUAL:
			status_m = _FS; // si es el separador pongo el writer en -1 por el post incremento de abajo
			writer=-1;;
			break;
		case QUESTION:
			writer=-1; // asi no se saltea una posicion por el ES cuando escribe
			status_m = _ES;
			break;
		default:
			status_m = _ERR;

	}
	writer++;

}

void write(char character , uint8_t state_m, uint8_t iterator_){
	switch(state_m){
		case (_PAN):
			myID.PAN[iterator_] = character;
			break;
		case (_FS):
			myID.ADD[iterator_] = character;
			break;

	}
}


bool magtek_iter(void){
	return magtek_interrupt;
}

bool magtek_clear(void){
	magtek_interrupt = false ;
	flush();
	return magtek_interrupt;
}

uint8_t* get_ID(void){
	return (myID.PAN);
}


void parse_PAN2NUM(void) {

	my_id_number = 0;
	for(uint8_t i=0; i<SIZE(myID.PAN) ; i++){
		my_id_number += my_id_number*10 + (myID.PAN[i] - '0');
	}
}
