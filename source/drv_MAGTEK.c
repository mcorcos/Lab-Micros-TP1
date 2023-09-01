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
	uint8_t data :5;
	uint8_t nc   :3;
}character;


typedef uint32_t id_number;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


//flags
static uint8_t state = _WAITING;
static uint8_t status = _SS;

//data
static uint8_t data[MAX_CHARS];

//contador
static uint16_t bit_counter = 0;
static uint16_t shift_counter = 0;
static uint16_t iterator = 0;
static uint16_t writer = 0;

//mis tipos de datos
static character mydata;
static ID myID;
static id_number my_id_number;

//event
static bool magtek_interrupt = false;


/*******************************************************************************
 * VARIABLE FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void flush(void);
void parse(void);
void parse_alphanumeric(character data);
void parse_PAN2NUM(void);
void write(char character , uint8_t state, uint8_t iterator);
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

  void initMagtek(){

	  //Pines necesarios

	  turnOn_GreenLed(); //Prendo led verde

	  gpioMode(PIN_MAGTEK_ENABLE,INPUT);
	  gpioMode(PIN_MAGTEK_CLOCK,INPUT);
	  gpioMode(PIN_MAGTEK_DATA,INPUT);


	  gpioIRQ(PIN_MAGTEK_ENABLE,PORT_eInterruptEither,ptrToEnable);
	  gpioIRQ(PIN_MAGTEK_CLOCK,PORT_eInterruptFalling,ptrToClock); //Por el moemnto solo necesito la interrupcion de clock



	  turnOff_GreenLed();//apago led verde




  }

void ptrToClock(void){
	if((state == _READING )){
	bool new_data = !(gpioRead(PIN_MAGTEK_DATA));
	data[bit_counter++] = new_data;
	}

}
 //ISR CUANDO EL ENABLE CAMBIE DE ESTADO

 void ptrToEnable(void){
	 turnOff_RedLed();
	 if((state == _WAITING)){
		 turnOn_BlueLed();
		state =_READING;
	 }
	 else if ((state == _READING ) ) {
		 turnOff_BlueLed();
		 turnOn_RedLed();
		 state = _END;
	 }
	 if(state == _END){
		 //interpretar
		 parse();


		 //prendo el flag!!!1
		 magtek_interrupt = true;

		 //ctadores en 0
		 bit_counter=0;
		 iterator =0;
		 writer =0;

		 //original states
		 status = _SS;
		 state = _WAITING;
	 }
 }


 void flush(void){
	 for(uint8_t i=0;i<SIZE(data);i++){
		 data[i] = 0;
	 }
	 for(uint8_t i=0;i<SIZE(myID.PAN);i++){
		 myID.PAN[i] = 0;
	 }
	 for(uint8_t i=0;i<SIZE(myID.PAN);i++){
		 myID.ADD[i] = 0;
	 }
 }

 void parse(void){

	 for(iterator=0; iterator<SIZE(data) && (status == _SS) ;iterator++){
		 mydata.data = (mydata.data<<1) | (data[iterator]);
		 if(mydata.data == MIRRORED_PUNTOCOMA){
			 status = _PAN;
		 }
	 }
	 if(status == _PAN){

		 mydata.data = 0b00000; //aranco con una palabra en 0
		 	 	 	 	 	 	 // El estado tiene que ser PAN , o FS      y el iterador tampoco se puede pasar de Data
		 for(shift_counter = 0; ( (status == _PAN) || (status == _FS)  )   && (iterator < MAX_CHARS)  ;       shift_counter++,iterator++){  //para cada character itero para formar la palabra

			 mydata.data = mydata.data | (data[iterator]<<shift_counter); // voy creando 0b00001 la palabra
			 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 //                    ^-----
			 if(shift_counter == (CHAR_LENGHT-1)){ // cuando el contador llega a la posicion 4 , pregunto que es la palabra
				 parse_alphanumeric(mydata);
				 mydata.data = 0b00000;
				 shift_counter = -1; //por el post incremento del for cuando vuelve me lo deja en 0
			 }
		 }
	}
	 if(status == _ES){ //tengo que conseguir el lrc
		 status = _SS;
	 }
	 if(status == _ERR){


#if ERROR_MODE && ERROR_CARD
		 turnOn_ErrorLed_1();
#endif
		 status = _SS;
	 }
}

void parse_alphanumeric(character data){
	switch(data.data){
		case CHAR_0:
			write('0',status,writer);
			break;
		case CHAR_1:
			write('1',status,writer);
			break;
		case CHAR_2:
			write('2',status,writer);
			break;
		case CHAR_3:
			write('3',status,writer);
			break;
		case CHAR_4:
			write('4',status,writer);
			break;
		case CHAR_5:
			write('5',status,writer);
			break;
		case CHAR_6:
			write('6',status,writer);
			break;
		case CHAR_7:
			write('7',status,writer);
			break;
		case CHAR_8:
			write('8',status,writer);
			break;
		case CHAR_9:
			write('9',status,writer);
			break;
		case PUNTOPUNTO:
			status = _ERR; //en los _ERR , no tendria que reibir nada de esto
			break;
		case PUNTOCOMA:
			status = _ERR;
			break;
		case MINOR:
			status = _ERR;
			break;
		case MAJOR:
			status = _ERR;
			break;
		case EQUAL:
			status = _FS; // si es el separador pongo el writer en -1 por el post incremento de abajo
			writer=-1;;
			break;
		case QUESTION:
			writer=-1; // asi no se saltea una posicion por el ES cuando escribe
			status = _ES;
			break;
		default:
			status = _ERR;

	}
	writer++;

}

void write(char character , uint8_t state, uint8_t iterator_){
	switch(state){
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
