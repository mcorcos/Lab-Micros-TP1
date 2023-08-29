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
#include "drv_K64.h"
#include "board.h"
#include "debug.h"
#include "gpio.h"

 /*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define CHAR_LENGHT (5)
#define MAX_CHARS (40)

#define SS 0b11010//;
#define ES 0b11111//?
#define FS 0b10110//=


#define SIZE(dat)  (sizeof(dat)/ sizeof(dat[0]))

 /*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct{
	uint8_t 	data 	:5;
	uint8_t 	nc		:3;
}mydata;



enum {_SS_,_PAN_,_FS_,_DATA_,_ES_ , _ERR_};
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

//data
static uint8_t ID[MAX_CHARS];

//flags
static bool read_data = false;
static uint8_t status;


//contadores
static uint8_t char_counter = 0;
static uint8_t bit_counter = 0;
static mydata my_data = {.data = 0b11111, .nc = 0b000};
static uint8_t k=0 ; //iterador del nuevo arreglo

static ptr_to_fun Fun;



/************************************************hola*******************************
 * VARIABLE FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void parseReadableData(uint8_t);
void initialize_data(void);
void parseData(void);

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

  void initMagtek(ptr_to_fun fun){

	  //Pines necesarios

	  gpioMode(PIN_MAGTEK_ENABLE,INPUT);
	  gpioMode(PIN_MAGTEK_CLOCK,INPUT);
	  gpioMode(PIN_MAGTEK_DATA,INPUT);

#if DEBUG_MODE && DEBUG_CARD
	  gpioMode(DEBUG_PIN_1,OUTPUT);
	  gpioWrite(DEBUG_PIN_1,LOW);
#endif

#if DEBUG_MODE && DEBUG_CARD
	  gpioMode(DEBUG_PIN_2,OUTPUT);
	  gpioWrite(DEBUG_PIN_2,LOW);
#endif

	  //gpioIRQ(PIN_MAGTEK_ENABLE,PORT_eInterruptEither,ptrToEnable);
	  gpioIRQ(PIN_MAGTEK_CLOCK,PORT_eInterruptFalling,ptrToClock); //Por el moemnto solo necesito la interrupcion de clock

	  Fun = fun;

	  //status init
	  status = _SS_;

	  //
	  read_data=false;
	  bit_counter=0;


  }


void ptrToClock(void){
	my_data.data =  (my_data.data<<1) | ( !(gpioRead(PIN_MAGTEK_DATA)));
	bit_counter++;
	if(bit_counter==CHAR_LENGHT){
		read_data=true;
		bit_counter=0;
	}
	parseData();
}


void parseData(void){

	if((status==_SS_) && (my_data.data == SS) ){

		status = _PAN_;
		read_data = false;
		bit_counter = 0;
		my_data.data = 0;
	}
	if ((status == _PAN_)  ){
		if(read_data==true){

			read_data=false;

			switch(my_data.data){
					 case 0b00001://0
						 ID[k]='0';
						break;
					 case 0b10000: //1
						 ID[k]='1';
						break;
					 case 0b01000://2
						 ID[k]='2';
						break;
					 case 0b11001://3
						 ID[k]='3';
						break;
					 case 0b00100://4
						 ID[k]='4';
						break;
					 case 0b10101://5
						 ID[k]='5';
						break;
					 case 0b01101://6
						 ID[k]='6';
						break;
					 case 0b11100://7
						 ID[k]='7';
						break;
					 case 0b00010://8
						 ID[k]='8';
						break;
					 case 0b10011://9
						 ID[k]='9';
						break;
					 case 0b01011://:
						 ID[k]='x';
						 status=_ERR_;
						break;
					 case 0b11010://;
						 ID[k]='x';
						 status=_ERR_;
						break;
					 case 0b00111://<
						 ID[k]='x';
						 status=_ERR_;
						break;
					 case 0b10110://=
						 ID[k]='F';
						 status = _FS_;
						 //k--;
						break;
					 case 0b01110://<
						 ID[k]='x';
						 status=_ERR_;
						break;
					 case 0b11111://?
						 ID[k]='E';
						 status = _ES_;
						// k--;
						break;
					 default:
						 ID[k]='X';
						 status=_ERR_;
						break;



					 }
			k++;
			my_data.data = 0;
		}
	}
	if (( (status == _ES_) || (status == _ERR_) )  ){
		if(status == _ES_ ){
			Fun(ID);
		}
		if(status == _ERR_){
			Fun(NULL);
		}
		initialize_data();
		k=0;
		status = _SS_;
	}
}


/* //ISR CUANDO EL ENABLE CAMBIE DE ESTADO

 void ptrToEnable(void){

	 enable = !gpioRead(PIN_MAGTEK_ENABLE);

	 if(status == _WAITING_){
		 initialize_data();
	 }
	 status++; //STATUS = 1 MEANS _READING_
	 if(status == _END_){
		 status = _WAITING_;
	 }

	 //contadores en 0
	 bit_counter = 0;
	 char_counter = 0;
	 character = 0;

	 //flags en false
	 read_data = false;
	 ss_received = false;
	 es_received = false;
	 card_ready = false;

  }*/



 void initialize_data(void){
	 for(uint8_t i=0;i<(SIZE(ID));i++){
		 ID[i] = 0;
		// readable_data[i]=0;
	 }
 }








 void parseReadableData(uint8_t data){

}

 void Function(const uint8_t* data){

 }




