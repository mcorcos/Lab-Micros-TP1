/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   G4
 ******************************************************************************/



/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "drv_MAGTEK.h"
#include "drv_K64.h"
#include "board.h"
#include "debug.h"
#include "gpio.h"

 /*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define _WAITING_ 0
#define _READING_ 1
#define _END_ 2

#define CHAR_LENGHT (5)
#define MAX_DATA (60)


#define SS 0b01011//;
#define ES 0b11111//?
#define FS 0b01101//=

#define SIZE(dat)  (sizeof(dat)/ sizeof(dat[0]))

 /*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef uint8_t card_char;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

static int enable = 0;
static int status = _WAITING_ ;

//data
static card_char data[MAX_DATA];
static card_char readable_data[MAX_DATA];

//flags
static bool read_data = false;
static bool ss_received = false;
static bool es_received = false;
static bool card_ready = false;

//contadores
static uint8_t char_counter = 0;
static uint8_t bit_counter = 0;
static card_char character = 0;
static int counter = 0;

//new data
static bool new_data = 0;


/************************************************hola*******************************
 * VARIABLE FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void arrayParser(void);
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

  void initMagtek(void){

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

	  gpioIRQ(PIN_MAGTEK_ENABLE,PORT_eInterruptEither,ptrToEnable);
	  gpioIRQ(PIN_MAGTEK_CLOCK,PORT_eInterruptFalling,ptrToClock);


  }




  //ISR CUANDO VENGA UN CLOCK DESCENDENTE

 void ptrToClock(void){
	 turnOn_RedLed();
	 new_data = !(gpioRead(PIN_MAGTEK_DATA)); // new bit
	 read_data = true;

	 //comienzo a armar el character
	 if( (read_data == true) &&  (bit_counter < CHAR_LENGHT) && (char_counter < MAX_DATA)  ){
		 character = character | (new_data << bit_counter); // pongo el bit donde vaya en el character
	 }
 	 if( (read_data == true)   &&   (bit_counter == (CHAR_LENGHT -1))  ){// llego a completar un char


 		 if((character == SS)   && (ss_received == false)     ){
 			 ss_received = true; //recibi el start sentinel
 		 }
 		 else if((character == ES)   && (es_received == false) &&   (ss_received == true)    ){
 			 es_received = true; //recibi el end sentinel
 		 }
 		 else if((es_received == true) &&   (ss_received == true) ){
 			card_ready = true; //La tarjeta esta lista para usarse
 		 }

 		 // dump el character en la palabra
 		 data[char_counter++] = character;

 		 // vovler a inicializar bit counter y character
 		 bit_counter = 0;
 		 character = 0;
 	 }
 	 else if(read_data==true){
 		 counter++;
 		 bit_counter++;
 	 }
 	turnOff_RedLed();

 }


 //ISR CUANDO EL ENABLE CAMBIE DE ESTADO

 void ptrToEnable(void){



	 enable = !gpioRead(PIN_MAGTEK_ENABLE);

	 if(status == _WAITING_){

		 turnOn_BlueLed(); // turn on white Led
		 turnOn_RedLed();
		 turnOn_GreenLed();

		 initialize_data();
	 }
	 status++; //STATUS = 1 MEANS _READING_
	 if(status == _END_){


		 turnOff_BlueLed();
		 turnOff_RedLed();
		 turnOff_GreenLed();


		 arrayParser();
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






  }



 void initialize_data(void){
	 for(uint8_t i=0;i<(SIZE(data));i++){
		 data[i] = 0;
		 readable_data[i]=0;
	 }
 }


 void arrayParser(void){
	 uint8_t i=0 ;
	 uint8_t k=0 ;

	 for(i=0,k=0 ;i<SIZE(data);i++){
		 switch(data[i]){
		 	 case 0b10000://0
		 		readable_data[k]='0';
		 		k++;
		 		break;
		 	 case 0b00001: //1
		 		readable_data[k]='1';
		 		k++;
		 		break;
		 	 case 0b00010://2
		 		readable_data[k]='2';
		 		k++;
		 		break;
		 	 case 0b10011://3
		 		readable_data[k]='3';
		 		k++;
		 		break;
		 	 case 0b00100://4
		 		readable_data[k]='4';
		 		k++;
		 		break;
		 	 case 0b10101://5
		 		readable_data[k]='5';
		 		k++;
		 		break;
		 	 case 0b10110://6
		 		readable_data[k]='6';
		 		k++;
		 		break;
		 	 case 0b00111://7
		 		readable_data[k]='7';
		 		k++;
		 		break;
		 	 case 0b01000://8
		 		readable_data[k]='8';
		 		k++;
		 		break;
		 	 case 0b11001://9
		 		readable_data[k]='9';
		 		k++;
		 		break;
		 	 case 0b11010://:
		 		readable_data[k]=':';
		 		k++;
		 		break;
		 	 case 0b01011://;
		 		readable_data[k]='S';
		 		k++;
		 		break;
		 	 case 0b11100://<
		 		readable_data[k]='<';
		 		k++;
		 		break;
		 	 case 0b01101://=
		 		readable_data[k]='F';
		 		k++;
		 		break;
		 	 case 0b01110://<
		 		readable_data[k]='>';
		 		k++;
		 		break;
		 	 case 0b11111://?
		 		readable_data[k]='E';
		 		k++;
		 		break;


		 }
	 }
 }



