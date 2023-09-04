/***************************************************************************//**
  @file     FSM.c
  @brief    state_fsm Machine
  @author   Grupo 4 Laboratorio de Microprocesadores:
            Corcos, Manuel
            Lesiuk, Alejandro
            Paget, Milagros
            Voss, Maria de Guadalupe
  ******************************************************************************/

  /*******************************************************************************
   * INCLUDE HEADER FILES
   ******************************************************************************/

#include "fsm.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "drv_ENCODER.h"
#include "drv_MAGTEK.h"
#include "drv_DEVBOARD.h"
#include "timer.h"
#include "display.h"

   //ME IMPORTA EL ESTADO ANTERIORR!!!!!!
   /*Estados de la máquina:
   * IDLE: estado en el que espero a que interactuen ("enter ID" msg)
   * PRE_ID: estado que espera mientras ingresan el ID por encoder
   * ID: estado que verifica que el ID este OK
   * ID_OK: estado que espera que interactuen "enter PIN"
   * PRE_PIN: estado que espera mientras ingresan el PIN por encoder
   * PIN: estado que verifica si el pin es correcto y da la señal de abrir la puerta
   * OPEN: abro la puerta y espero 5 seg
   * ADMIN: estado que se entra si el usuario es admin
   * ADD: estado que puede acceder el ADMIN para agregar un usuario
   * REMOVE: estado que puede acceder el ADMIN o si el error == 3
   * TIMEOUT: estado al que se llega si se excede el tiempo de no interacción en algun estado
   * ERROR: estado al que se llega si hay algun error, ya sea PIN o ID */

   /* Variables de la máquina
   * waiting_id: 1 si estoy en PRE_ID
   * waiting_pin: 1 si estoy en PRE_PIN
   */

   /*******************************************************************************
    *  CONSTANT AND MACRO DEFINITIONS
    ******************************************************************************/
#define IDLE 0
#define PRE_ID 1
#define ID 2
#define ID_OK 3
#define PRE_PIN 4
#define PIN 5
#define OPEN 6
#define ADMIN 7
#define ADD 8
#define NEW_ID 9
#define NEW_PIN 10
#define NEW_ADMIN 11
#define REMOVE 12
#define REMOVE_ID 13
#define TIMEOUT 14
#define ERROR 15
#define BRIGHT 16
#define MENU 17
#define CHANGE_PASSWORD 18
#define NEW_ID_OK 19

    //Definiciones de largos de datos
#define NUM_ID 8
#define NUM_PIN 4
#define NUM_USERS 10


//def para timer
#define _USING 1
#define _NOT_USING 0

//Definiciones de acciones del display
#define WAITING 0
#define MSG_SENT 1

//Definiciones de acciones con el encoder
#define OK 1
#define BACK 2
#define BRIGHTNESS 3
#define RESTART 4

#define TURN 1
#define BUTTON 2


//Posibles atributos de los usuarios
#define NOT_ADMIN_ATT 0
#define ADMIN_ATT 1

#define ADMIN_OPEN 1
#define ADMIN_CHANGE_PASSWORD 2
#define ADMIN_ADD 3
#define ADMIN_REMOVE 4
#define ADMIN_BRIGHT 5
#define ADMIN_RESTART 6




//definees de tiempo
//Defines para el timer
#define TIMEOUT_PRE_ID 90000
#define TIMEOUT_PRE_PIN 45000
#define TIMEOUT_ID_OK 30000
#define TIMEOUT_MENU 60000
#define TIMEOUT_CHANGE_PASSWORD 45000
#define TIMEOUT_OPEN 5000
#define TIMEOUT_ADMIN 45000
#define TIMEOUT_NEW_ID 90000
#define TIMEOUT_NEW_ID_OK 30000
#define TIMEOUT_NEW_PIN 45000
#define TIMEOUT_NEW_ADMIN 25000
#define TIMEOUT_REMOVE 40000
#define TIMEOUT_TIMEOUT 4000
#define TIMEOUT_ERROR 1500
#define TIMEOUT_BRIGHTNESS 40000
/*******************************************************************************
 *  ENUMERATIONS, STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct {
    char id[8];
    char pin[4];
    int attribute;
    int attempts;
}user_t;

/*******************************************************************************
 *  VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
static user_t users[] = {
    {{'5', '1', '7', '7', '5', '6' ,'4','8'},{'1','8','6','0'}, ADMIN_ATT, 0},
    {{'7', '8', '7', '8', '8', '5', '2', '6'}, {'3','1','0','3'}, NOT_ADMIN_ATT, 0},
    {{0}, {0}, 0, 0},
    {{0}, {0}, 0, 0},
    {{0}, {0}, 0, 0},
    {{0}, {0}, 0, 0},
    {{0}, {0}, 0, 0},
    {{0}, {0}, 0, 0},
    {{0}, {0}, 0, 0},
    {{0}, {0}, 0, 0}
};

static ENCODER_Type encoder;

static tim_id_t timer;

static uint8_t encoder_inter;

static bool magtek_inter;

static uint8_t pre_state_fsm = 0;
static uint8_t state_fsm = 0;


static bool using_timer = false;

static uint8_t waiting_id = 0;
static uint8_t  aux_user_count = 0;
static int8_t user_count = 0;
static uint8_t msg_sent = WAITING;
static uint8_t i = 0;
static uint8_t  waiting_pin = 0;
static uint8_t error = 0;

static char temp_pin[] = "0000", temp_id[] = "0000000000000000000";
/*******************************************************************************
 *  FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

 /**
  * @brief Verifies that the entered ID exists in the database.
  * @param The ID that needs to be verified.
  * @return number of id in the array or -1 if it does not exist.
  */
int verify_id(char data[]);


/**
 * @brief Verifies that the entered PIN is correct for the user being used.
 * @param data is the PIN that needs to be verified.
 * @param user_num is the order in the array of this user.
 * @return 1 if it is correct or 0 if it is not.
 */
int verify_pin(char data[], int user_num);

/**
 * @brief Verifies if a new user can be added.
 * @param void.
 * @return Returns -1 if no new users are allowed, or the user number that can be used.
 */
int check_users(void);

/**
 * @brief Verifies if a new user can be added.
 * @param data contains user information.
 * @param user num is the number in the array.
 * @return void.
 */
void enter_new_user(user_t data, int user_num);

/**
 * @brief Removes a user from database.
 * @param user_num is the number in the array.
 * @return void.
 */
void remove_user(int user_num);

/**
 * @brief Gets number from encoder information.
 * @param void.
 * @return returns the number.
 */
char get_number(void);

/**
 * @brief Callback for timer.
 * @param void.
 * @return void.
 */
void callback_timer(void);

/**
 * @brief Changes user password.
 * @param new_pass contains the new password for the user.
 * @return void.
 */
void change_password(char new_pass[], int user_count);

/*******************************************************************************
 *  GLOBAL FUNCTIONS DEFINITIONS // finish
 ******************************************************************************/
int fsm(void) {

    encoder_inter = encoderInterrup();
    if (encoder_inter) {
        encoder = getEncoder();
    }
    magtek_inter = magtek_iter();

    static user_t data = { {'0'}, {0}, 0, 0 }; //Variable que almacena la información ingresada

    if(!timer){
        timer = timerGetId();
    }
    uint8_t* ptr;

    switch (state_fsm) {
    case IDLE: {
        if (encoder_inter && !magtek_inter) {
            //Si hubo una interacción con el encoder quiere decir que me ingresan el ID por el encoder.
            //Paso al estado PRE_ID y reinicio el contador.
            pre_state_fsm = state_fsm;
            if (encoder_inter == BUTTON) {
                if(encoder.cantPresion == BRIGHTNESS){
                	pre_state_fsm = state_fsm;
                	state_fsm = BRIGHT;
                	clearEncoderPresiones();
                }
                else{
                	state_fsm = PRE_ID;
					waiting_id = 1;
					msg_sent = WAITING;
					clearEncoderPresiones();
                }
            }
            clearEncoderGiros();
            encoder_inter = clearEncoderInter();
        }
        else if (magtek_inter && !encoder_inter) {
            //Si hubo una interacción con la magtek quiere decir que me ingresan el ID con tarjeta.
            //Paso al estado ID y reinicio el contador.
            pre_state_fsm = state_fsm;
            state_fsm = ID;
            ptr = get_ID();
            ptr += 8;
            for (int i = 0; i < NUM_ID; i++) {
                data.id[i] = *ptr;
                ++ptr;
            }
            magtek_inter = magtek_clear();
            msg_sent = WAITING;
        }
        else if (magtek_inter && encoder_inter) {
            //No debería haber dos interacciones a la vez.
            //Paso al estado ERROR y reinicio el contador.
            pre_state_fsm = state_fsm;
            state_fsm = ERROR;
            magtek_inter = magtek_clear();
            encoder_inter = clearEncoderInter();
            clearEncoderPresiones();
            clearEncoderGiros();
            msg_sent = WAITING;
        }
        else {
            pre_state_fsm = state_fsm;
            state_fsm = state_fsm;
            if (!msg_sent) {
                print_msg("Enter ID", MOVE); //---> FUNCION DEL DISPLAY.
                msg_sent = MSG_SENT;
            }
        }
        for(int j = 0; j < NUM_PIN; j++){
        	temp_pin[j] = '0';
        }
        for(int j = 0; j < NUM_ID; j++){
			temp_id[j] = '0';
		}
        break;
    }
    case PRE_ID: {
        //Estado en el que espero que ingresen todo el ID. Esto es si me mandan dígito por dígito.
        //Ver como me mandan los dígitos, si es que necesito un id_count para ver cuantos dígitos ingresaron o algo así.
    	if ((!using_timer) &&( !timerRunning(timer))) {
    		using_timer = _USING;
            timerStart(timer, TIMER_MS2TICKS(TIMEOUT_PRE_ID), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = TIMEOUT;
                msg_sent = WAITING;
				using_timer = _NOT_USING;
            }
            else {
                if (magtek_inter) {
                    pre_state_fsm = state_fsm;
                    magtek_inter = magtek_clear();
                }
                if (encoder_inter) {
					if(encoder.cantPresion == BRIGHTNESS){
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						timerStop(timer);
						using_timer = _NOT_USING;
						clearEncoderPresiones();
					}
					else if (waiting_id) {
                        pre_state_fsm = state_fsm;
                        if (encoder.cantPresion == OK) {
                        	msg_sent = WAITING; //si esto anda bien hay que agregarlo
                            data.id[i] = get_number();
                            temp_id[i] = data.id[i];
                            ++i;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                            if(i != NUM_ID){
								temp_id[i] = '0';
							}
                        }
                        else if (encoder.cantPresion == BACK) {
                            if (i == 0) {
                                pre_state_fsm = state_fsm;
                                i = 0;
                            }
                            else {
                                --i;
                            }
                            msg_sent = WAITING;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }
                        else if (encoder.cantPresion == RESTART) {
                            i = 0;
                            pre_state_fsm = state_fsm;
                            state_fsm = IDLE;
                            msg_sent = WAITING;
                            timerStop(timer);
                            using_timer = _NOT_USING;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }

                        if (i == NUM_ID) {
                            waiting_id = 0;
                            i = 0;
                            for(int j = 0; j < NUM_ID; j++){
								temp_id[j] = '0';
							}
                        }
                    }
					if(encoder_inter == TURN){
						msg_sent = WAITING;
						temp_id[i] = get_number();
					}
					encoder_inter = clearEncoderInter();
                }
                if(!waiting_id) {
					pre_state_fsm = state_fsm;
					state_fsm = ID;
					msg_sent = WAITING;
					timerStop(timer);
					using_timer = _NOT_USING;
				}
                else if((!msg_sent) && (state_fsm != IDLE)){
                	print_msg(temp_id, i);
					msg_sent = MSG_SENT;
				}
            }
        }
        break;
    }
    case ID: {
        user_count = verify_id(data.id);
			if (user_count != -1) {
				pre_state_fsm = state_fsm;
				state_fsm = ID_OK;

				msg_sent = WAITING;
			}
			else {
				pre_state_fsm = state_fsm;
				state_fsm = ERROR;
				user_count = 0;
				msg_sent = WAITING;
			}
        break;
    }
    case ID_OK: {
        if ((!using_timer) &&( !timerRunning(timer))) {
        	using_timer = _USING;
            timerStart(timer, TIMER_MS2TICKS(TIMEOUT_ID_OK), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
            	using_timer = _NOT_USING;
                pre_state_fsm = state_fsm;
                state_fsm = TIMEOUT;
                msg_sent = WAITING;
            }
            else {
                if (magtek_inter) {
                    magtek_inter = magtek_clear();
                }
                if (encoder_inter) {
                    pre_state_fsm = state_fsm;
                    if (encoder_inter == BUTTON) {
                    	if(encoder.cantPresion == BRIGHTNESS){
							pre_state_fsm = state_fsm;
							state_fsm = BRIGHT;
							timerStop(timer);
							using_timer = _NOT_USING;
							clearEncoderPresiones();
						}
                    	else{
                            state_fsm = PRE_PIN;
                            waiting_pin = 1;
                            msg_sent = WAITING;
                            timerStop(timer);
                            using_timer = _NOT_USING;
                            clearEncoderPresiones();
                    	}
                    }
                    encoder_inter = clearEncoderInter();
                    clearEncoderGiros();
                }
                else {
                    pre_state_fsm = state_fsm;
                    state_fsm = state_fsm;
                    if (!msg_sent) {
                        //Acá debería haber algo así como
                        print_msg("Enter PIN", MOVE); //---> FUNCION DEL DISPLAY.
                        msg_sent = MSG_SENT;
                    }
                }
            }
        }

        break;
    }
    case PRE_PIN: {
        if ((!using_timer) &&( !timerRunning(timer))) {
        	using_timer = _USING;
            timerStart(timer, TIMER_MS2TICKS(TIMEOUT_PRE_PIN), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
            	using_timer = _NOT_USING;
                pre_state_fsm = state_fsm;
                state_fsm = TIMEOUT;
                msg_sent = WAITING;
            }
            else {
                if (magtek_inter) {
                    pre_state_fsm = state_fsm;
                    magtek_inter = magtek_clear();
                }
                if (encoder_inter) {
                	if(encoder.cantPresion == BRIGHTNESS){
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						clearEncoderPresiones();
						timerStop(timer);
						using_timer = _NOT_USING;
					}
                	else if (waiting_pin) {
                        pre_state_fsm = state_fsm;
                        if (encoder.cantPresion == OK) {
                        	msg_sent = WAITING;
                            data.pin[i] = get_number();
                            temp_pin[i] = '-';
                            ++i;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                            if(i != NUM_PIN){
                            	temp_pin[i] = '0';
                            }
                        }
                        else if (encoder.cantPresion == BACK) {
                            if (i == 0) {
                                pre_state_fsm = state_fsm;
                                i = 0;
                            }
                            else {
                                --i;
                            }
                            msg_sent = WAITING;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }
                        else if (encoder.cantPresion == RESTART) {
                            i = 0;
                            pre_state_fsm = state_fsm;
                            state_fsm = IDLE;
                            msg_sent = WAITING;
                            timerStop(timer);
                            using_timer = _NOT_USING;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }

                        if (i == NUM_PIN) {
							waiting_pin = 0;
							i = 0;
							for(int j = 0; j < NUM_PIN; j++){
								temp_pin[j] = '0';
							}
							clearEncoderGiros();
							clearEncoderPresiones();
						}
                    }
                	if(encoder_inter == TURN){
						msg_sent = WAITING;
						temp_pin[i] = get_number();
					}
                    encoder_inter = clearEncoderInter();
				}
                if(!waiting_pin) {
                    pre_state_fsm = state_fsm;
                    state_fsm = PIN;
                    msg_sent = WAITING;
                    timerStop(timer);
                    using_timer = _NOT_USING;
                    i = 0;
                }
                else if((!msg_sent)&& (state_fsm != IDLE)){
					print_msg(temp_pin, MOVE);
					msg_sent = MSG_SENT;
				}
            }
        }
        break;
    }
    case PIN: {
        if (verify_pin(data.pin, user_count)) {
            pre_state_fsm = state_fsm;
            users[user_count].attempts = 0;
            if (users[user_count].attribute) {
                state_fsm = ADMIN;
            }
            else {
                state_fsm = MENU;
            }
            msg_sent = WAITING;
        }
        else {
            if (users[user_count].attempts >= 3) {
                remove_user(user_count);
                pre_state_fsm = state_fsm;
                state_fsm = ERROR;
                user_count = 0;
            }
            else {
                pre_state_fsm = state_fsm;
                state_fsm = ID_OK;
            }
            msg_sent = WAITING;
        }
        break;
    }
    case MENU:{
    	if ((!using_timer) &&( !timerRunning(timer))) {
    		using_timer = _USING;
			timerStart(timer, TIMER_MS2TICKS(TIMEOUT_MENU), TIM_MODE_SINGLESHOT, callback_timer);
		}
		else {
			if (timerExpired(timer)) {

				pre_state_fsm = state_fsm;
				state_fsm = IDLE;
				user_count = 0;
				msg_sent = WAITING;
				using_timer = _NOT_USING;
			}
			else {
				if (magtek_inter) {
					pre_state_fsm = state_fsm;
					magtek_inter = magtek_clear();
				}
				if (encoder_inter) {
					if (encoder.cantPresion == 1) {
						pre_state_fsm = state_fsm;
						state_fsm = OPEN;
						msg_sent = WAITING;
						timerStop(timer);
						using_timer = _NOT_USING;
						clearEncoderPresiones();
						clearEncoderGiros();
					}
					else if (encoder.cantPresion == 2) {
						pre_state_fsm = state_fsm;
						state_fsm = CHANGE_PASSWORD;
						timerStop(timer);
						using_timer = _NOT_USING;
						msg_sent = WAITING;
						clearEncoderPresiones();
						clearEncoderGiros();
						waiting_pin = 1;
					}
					else if (encoder.cantPresion == RESTART) {
						pre_state_fsm = state_fsm;
						state_fsm = IDLE;
						msg_sent = WAITING;
						timerStop(timer);
						using_timer = _NOT_USING;
						clearEncoderPresiones();
						clearEncoderGiros();
					}
					else if (encoder.cantPresion == BRIGHTNESS) {
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
						using_timer = _NOT_USING;
						clearEncoderPresiones();
						clearEncoderGiros();
					}
					encoder_inter = clearEncoderInter();
				}
				else if((!msg_sent)&& (state_fsm != IDLE)){
					print_msg("1 to open 2 to change pin", MOVE);
					msg_sent = MSG_SENT;
				}
			}
		}
		break;
    }
    case CHANGE_PASSWORD:{
    	if ((!using_timer) &&( !timerRunning(timer))) {
    		using_timer = _USING;
			timerStart(timer, TIMER_MS2TICKS(TIMEOUT_CHANGE_PASSWORD), TIM_MODE_SINGLESHOT, callback_timer);
		}
		else {
			if (timerExpired(timer)) {
				pre_state_fsm = state_fsm;
				state_fsm = IDLE;
				user_count = 0;
				msg_sent = WAITING;
				using_timer = _NOT_USING;
			}
			else {
				if (magtek_inter || encoder_inter) {
					magtek_inter = magtek_clear();
					pre_state_fsm = state_fsm;
					if (encoder.cantPresion == BRIGHTNESS) {
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
						using_timer = _NOT_USING;
						clearEncoderPresiones();
					}
					else if (waiting_pin) {
						pre_state_fsm = state_fsm;
						if (encoder.cantPresion == OK) {
							msg_sent = WAITING;
							data.pin[i] = get_number();
							++i;
							clearEncoderPresiones();
							clearEncoderGiros();
							if(i != NUM_PIN){
								temp_pin[i] = '0';
							}
						}
						else if (encoder.cantPresion == BACK) {
							if (i == 0) {
								pre_state_fsm = state_fsm;
								i = 0;
							}
							else {
								--i;
							}
							msg_sent = WAITING;
							clearEncoderPresiones();
							clearEncoderGiros();
						}
						else if (encoder.cantPresion == RESTART) {
							i = 0;
							pre_state_fsm = state_fsm;
							state_fsm = IDLE;
							msg_sent = WAITING;
							timerStop(timer);
							using_timer = _NOT_USING;
							clearEncoderPresiones();
							clearEncoderGiros();
						}

						if(encoder_inter == TURN){
							msg_sent = WAITING;
							temp_pin[i] = get_number();
						}

						if (i == NUM_PIN) {
							waiting_pin = 0;
							i = 0;
							for(int j = 0; j < NUM_PIN; j++){
								temp_pin[j] = '0';
							}
						}

					}
					encoder_inter = clearEncoderInter();
				}
				if(!waiting_pin){
					change_password(data.pin, user_count);
					if(users[user_count].attribute == ADMIN_ATT){
						state_fsm = ADMIN;
						timerStop(timer);
						using_timer = _NOT_USING;
					}
					else{
						state_fsm = MENU;
						timerStop(timer);
						using_timer = _NOT_USING;
					}
					msg_sent = WAITING;
				}
				else if((!msg_sent)&& (state_fsm != IDLE)){
					msg_sent = MSG_SENT;
					print_msg(temp_pin, MOVE);
				}
			}
		}
		break;
    }
    case OPEN: {
        if ((!using_timer) &&( !timerRunning(timer))) {
        	using_timer = _USING;
            timerStart(timer, TIMER_MS2TICKS(TIMEOUT_OPEN), TIM_MODE_SINGLESHOT, callback_timer);
            turnOn_DebugLed_2();
            turnOn_RedLed();
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
                using_timer = _NOT_USING;
                turnOff_DebugLed_2();
                turnOff_RedLed();
            }
            else {
                if (magtek_inter || encoder_inter == BUTTON) {
                	if(encoder.cantPresion == BRIGHTNESS){
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						timerStop(timer);
						using_timer = _NOT_USING;
					}
                	clearEncoderPresiones();
                	clearEncoderGiros();
                	encoder_inter = clearEncoderInter();
                	magtek_inter = magtek_clear();
                }

                if (!msg_sent) {
                    print_msg("DOOR OPEN", MOVE); //---> FUNCION DEL DISPLAY.
                    msg_sent = MSG_SENT;
                }
            }
        }
        break;
    }
    case ADMIN: {
        if ((!using_timer) &&( !timerRunning(timer))) {
        	using_timer = _USING;
            timerStart(timer, TIMER_MS2TICKS(TIMEOUT_ADMIN), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
                using_timer = _NOT_USING;
            }
            else {
                if (magtek_inter) {
                    pre_state_fsm = state_fsm;
                    magtek_inter = magtek_clear();
                }
                if (encoder_inter) {
                    if (encoder.cantPresion == ADMIN_OPEN) {
                        pre_state_fsm = state_fsm;
                        state_fsm = OPEN;
                        msg_sent = WAITING;
                        timerStop(timer);
                        using_timer = _NOT_USING;
                        clearEncoderPresiones();
                        clearEncoderGiros();
                    }
                    else if (encoder.cantPresion == ADMIN_CHANGE_PASSWORD) {
						pre_state_fsm = state_fsm;
						state_fsm = CHANGE_PASSWORD;
						timerStop(timer);
						using_timer = _NOT_USING;
						msg_sent = WAITING;
						clearEncoderPresiones();
						clearEncoderGiros();
						waiting_pin = 1;
					}
                    else if (encoder.cantPresion == ADMIN_ADD) {
                        pre_state_fsm = state_fsm;
                        state_fsm = ADD;
                        timerStop(timer);
                        using_timer = _NOT_USING;
                        msg_sent = WAITING;
                        clearEncoderPresiones();
                        clearEncoderGiros();
                    }
                    else if (encoder.cantPresion == ADMIN_REMOVE) {
                        pre_state_fsm = state_fsm;
                        state_fsm = REMOVE;
                        timerStop(timer);
                        using_timer = _NOT_USING;
                        msg_sent = WAITING;
                        waiting_id = 1;
                        clearEncoderPresiones();
                       	clearEncoderGiros();
                    }
                    else if (encoder.cantPresion == ADMIN_RESTART) {
                        pre_state_fsm = state_fsm;
                        state_fsm = IDLE;
                        msg_sent = WAITING;
                        timerStop(timer);
                        using_timer = _NOT_USING;
						clearEncoderPresiones();
						clearEncoderGiros();
                    }
                    else if (encoder.cantPresion == ADMIN_BRIGHT) {
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
						using_timer = _NOT_USING;
						clearEncoderPresiones();
						clearEncoderGiros();
					}
                    encoder_inter = clearEncoderInter();
                }
                else if((!msg_sent)&& (state_fsm != IDLE)){
                	print_msg("1 to open 2 to change pin 3 to add 4 to remove", MOVE);
                	msg_sent = MSG_SENT;
                }
            }
        }
        break;
    }
    case ADD: {
        aux_user_count = check_users();
        if (aux_user_count != -1) {
            pre_state_fsm = state_fsm;
            state_fsm = NEW_ID;
            if (!msg_sent) {
                print_msg("Enter New ID", MOVE);
                msg_sent = MSG_SENT;
            }
            waiting_id = 1;
        }
        else {
            pre_state_fsm = state_fsm;
            state_fsm = ERROR;
            msg_sent = WAITING;
            timerStop(timer);
            using_timer = _NOT_USING;
        }
        break;
    }
    case NEW_ID: {
        if ((!using_timer) &&( !timerRunning(timer))) {
        	using_timer = _USING;
            timerStart(timer, TIMER_MS2TICKS(TIMEOUT_NEW_ID), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
            	using_timer = _NOT_USING;
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
            }
            else if (magtek_inter){

				ptr = get_ID();
				ptr+=8;
				for (int i = 0; i < NUM_ID; i++) {
					data.id[i] = *ptr;
					++ptr;
				}
				magtek_inter = magtek_clear();
            	timerStop(timer);
            	using_timer = _NOT_USING;
            	pre_state_fsm = state_fsm;
				if(verify_id(data.id) == -1){
					state_fsm = NEW_ID_OK;
				}
				else{
					state_fsm = ADMIN;
				}
				waiting_id = 0;
            }
            else if(encoder_inter) {
                pre_state_fsm = state_fsm;
                if (encoder.cantPresion == BRIGHTNESS) {
					pre_state_fsm = state_fsm;
					state_fsm = BRIGHT;
					msg_sent = WAITING;
					timerStop(timer);
					using_timer = _NOT_USING;
					clearEncoderPresiones();
				}
                else if (waiting_id) {
                    if (encoder.cantPresion == OK) {
                    	msg_sent = WAITING;
                        data.id[i] = get_number();
                        temp_id[i] = data.id[i];
                        ++i;
                        clearEncoderPresiones();
                        clearEncoderGiros();
                        if(i != NUM_ID){
							temp_id[i] = '0';
						}
                    }
                    else if (encoder.cantPresion == BACK) {
                        if (i == 0) {
                            pre_state_fsm = state_fsm;
                            i = 0;
                        }
                        else {
                            --i;
                        }
                        msg_sent = WAITING;
                        clearEncoderPresiones();
                        clearEncoderGiros();
                    }
                    else if (encoder.cantPresion == RESTART) {
                        i = 0;
                        pre_state_fsm = state_fsm;
                        state_fsm = IDLE;
                        msg_sent = WAITING;
                        clearEncoderPresiones();
                        clearEncoderGiros();
                        timerStop(timer);
                        using_timer = _NOT_USING;
                    }

                    if (i == NUM_ID) {
                        waiting_id = 0;
                        i = 0;
                        for(int j = 0; j < NUM_ID; j++){
							temp_id[j] = '0';
						}
                    }
                }
                if(encoder_inter == TURN){
					msg_sent = WAITING;
					temp_id[i] = get_number();
				}
                encoder_inter = clearEncoderInter();
            }

            if(!waiting_id){
            	state_fsm = NEW_ID_OK;
            	if(verify_id(data.id) == -1){
					state_fsm = NEW_ID_OK;
				}
				else{
					state_fsm = ADMIN;
				}
				msg_sent = WAITING;
				waiting_pin = 1;
				timerStop(timer);
				using_timer = _NOT_USING;
            }
            else if((!msg_sent)&& (state_fsm != ADMIN)){
            	msg_sent = MSG_SENT;
            	print_msg(temp_id, i);
            }
        }
        break;
    }
    case NEW_ID_OK:{
    	if ((!using_timer) &&( !timerRunning(timer))) {
    		using_timer = _USING;
			timerStart(timer, TIMER_MS2TICKS(TIMEOUT_NEW_ID_OK), TIM_MODE_SINGLESHOT, callback_timer);
		}
		else {
			if (timerExpired(timer)) {
				pre_state_fsm = state_fsm;
				state_fsm = TIMEOUT;
				msg_sent = WAITING;
				using_timer = _NOT_USING;
			}
			else {
				if (magtek_inter) {
					magtek_inter = magtek_clear();
				}
				if (encoder_inter) {
					pre_state_fsm = state_fsm;
					if (encoder_inter == BUTTON) {
						if(encoder.cantPresion == BRIGHTNESS){
							pre_state_fsm = state_fsm;
							state_fsm = BRIGHT;
							timerStop(timer);
							using_timer = _NOT_USING;
							clearEncoderPresiones();
						}
						else{
							state_fsm = NEW_PIN;
							waiting_pin = 1;
							msg_sent = WAITING;
							timerStop(timer);
							using_timer = _NOT_USING;
							clearEncoderPresiones();
						}
					}
					encoder_inter = clearEncoderInter();
					clearEncoderGiros();
				}
				else {
					pre_state_fsm = state_fsm;
					state_fsm = state_fsm;
					if (!msg_sent) {
						//Acá debería haber algo así como
						print_msg("Enter new PIN", MOVE); //---> FUNCION DEL DISPLAY.
						msg_sent = MSG_SENT;
					}
				}
			}
		}

    	break;
    }
    case NEW_PIN: {
        if ((!using_timer) &&( !timerRunning(timer))) {
        	using_timer = _USING;
            timerStart(timer, TIMER_MS2TICKS(TIMEOUT_NEW_PIN), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
                using_timer = _NOT_USING;
            }
            else {
                if (magtek_inter || encoder_inter) {
                    magtek_inter = magtek_clear();
                    pre_state_fsm = state_fsm;
                    if (encoder.cantPresion == BRIGHTNESS) {
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
						using_timer = _NOT_USING;
						clearEncoderPresiones();
					}
                    else if (waiting_pin) {
                        pre_state_fsm = state_fsm;
                        if (encoder.cantPresion == OK) {
                        	msg_sent = WAITING;
                            data.pin[i] = get_number();
                            temp_pin[i] = '-';
                            ++i;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                            if(i != NUM_PIN){
								temp_pin[i] = '0';
							}
                        }
                        else if (encoder.cantPresion == BACK) {
                            if (i == 0) {
                                pre_state_fsm = state_fsm;
                                i = 0;
                            }
                            else {
                                --i;
                            }
                            msg_sent = WAITING;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }
                        else if (encoder.cantPresion == RESTART) {
                            i = 0;
                            pre_state_fsm = state_fsm;
                            state_fsm = IDLE;
                            msg_sent = WAITING;

                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }

                        if(encoder_inter == TURN){
                        	msg_sent = WAITING;
        					temp_pin[i] = get_number();
                        }

                        if (i == NUM_PIN) {
                            waiting_pin = 0;
                            i = 0;
                            for(int j = 0; j < NUM_PIN; j++){
								temp_pin[j] = '0';
							}
                        }

                    }
                    encoder_inter = clearEncoderInter();
                }
                if(!waiting_pin){
                	state_fsm = NEW_ADMIN;
					msg_sent = WAITING;
					if (!msg_sent) {
						print_msg("1 if admin 2 if not", MOVE);
						msg_sent = MSG_SENT;
					}
					timerStop(timer);
					using_timer = _NOT_USING;
				}
				else if((!msg_sent)&& (state_fsm != IDLE)){
					msg_sent = MSG_SENT;
					print_msg(temp_pin, MOVE);
				}
            }
        }
        break;
    }
    case NEW_ADMIN: {
        if ((!using_timer) &&( !timerRunning(timer))) {
        	using_timer = _USING;
            timerStart(timer, TIMER_MS2TICKS(TIMEOUT_NEW_ADMIN), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
                using_timer = _NOT_USING;
            }
            else {
                if (encoder_inter || magtek_inter) {
                    magtek_inter = magtek_clear();
                    if (encoder.cantPresion == OK) {
                        data.attribute = ADMIN_ATT;
                        enter_new_user(data, aux_user_count);
                        pre_state_fsm = state_fsm;
                        state_fsm = ADMIN;
                        msg_sent = WAITING;
                        timerStop(timer);
                        using_timer = _NOT_USING;
                    }
                    else if (encoder.cantPresion == 2) {
                        data.attribute = NOT_ADMIN_ATT;
                        enter_new_user(data, aux_user_count);
                        pre_state_fsm = state_fsm;
                        state_fsm = ADMIN;
                        msg_sent = WAITING;
                        timerStop(timer);
                        using_timer = _NOT_USING;
                    }
                    else if (encoder.cantPresion == RESTART) {
                        i = 0;
                        pre_state_fsm = state_fsm;
                        state_fsm = IDLE;
                        msg_sent = WAITING;
                        timerStop(timer);
                        using_timer = _NOT_USING;
                    }
                    else if (encoder.cantPresion == BRIGHTNESS) {
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
						using_timer = _NOT_USING;
					}
                    clearEncoderPresiones();
                    clearEncoderGiros();
                    clearEncoderInter();
                }
                else {
                    pre_state_fsm = state_fsm;
                }
            }
        }
        break;
    }
    case REMOVE: {
        if ((!using_timer) &&( !timerRunning(timer))) {
        	using_timer = _USING;
            timerStart(timer, TIMER_MS2TICKS(TIMEOUT_REMOVE), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
                using_timer = _NOT_USING;
            }
            else {
                if (encoder_inter || magtek_inter) {
                    magtek_inter = magtek_clear();
                    if (encoder.cantPresion == BRIGHTNESS) {
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
						using_timer = _NOT_USING;
						clearEncoderPresiones();
					}
                    else if (waiting_id) {
                        pre_state_fsm = state_fsm;
                        if (encoder.cantPresion == OK) {
                            aux_user_count = get_number()-'0';
                            ++i;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                            temp_pin[3] = aux_user_count + '0';
                        }
                        else if (encoder.cantPresion == BACK) {
                            if (i == 0) {
                                pre_state_fsm = state_fsm;
                                i = 0;
                            }
                            else {
                                --i;
                            }
                            msg_sent = WAITING;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }
                        else if (encoder.cantPresion == RESTART) {
                            i = 0;
                            pre_state_fsm = state_fsm;
                            state_fsm = IDLE;
                            msg_sent = WAITING;
                            timerStop(timer);
                            using_timer = _NOT_USING;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }
                        if(encoder_inter == TURN){
							msg_sent = WAITING;
							temp_pin[3] = get_number();
						}

                        if (i == 1) {
                            waiting_id = 0;
                            i = 0;
                            for(int j = 0; j < NUM_PIN; j++){
								temp_pin[j] = '0';
							}
                        }
                    }
                    if(!waiting_id){
						state_fsm = REMOVE_ID;
						pre_state_fsm = state_fsm;
						msg_sent = WAITING;
						timerStop(timer);
						using_timer = _NOT_USING;
					}
					else if((!msg_sent)&& (state_fsm != IDLE)){
						msg_sent = MSG_SENT;
						print_msg(temp_pin, MOVE);
					}
                    encoder_inter = clearEncoderInter();
                }
            }
        }
        break;
    }
    case REMOVE_ID: {
        if (aux_user_count == user_count) {
            pre_state_fsm = state_fsm;
            state_fsm = ERROR;
            msg_sent = WAITING;
        }
        else {
            remove_user(aux_user_count);
            aux_user_count = 0;
            pre_state_fsm = state_fsm;
            state_fsm = ADMIN;
            msg_sent = WAITING;
        }
        break;
    }
    case TIMEOUT: {
    	if ((!using_timer) &&( !timerRunning(timer))) {
    		using_timer = _USING;
			timerStart(timer, TIMER_MS2TICKS(TIMEOUT_TIMEOUT), TIM_MODE_SINGLESHOT, callback_timer);
		}
		else {
			if (timerExpired(timer)) {
				using_timer = _NOT_USING;
				pre_state_fsm = state_fsm;
				state_fsm = IDLE;
				msg_sent = WAITING;
			}
			else if (!msg_sent) {
				print_msg("Timeout", MOVE);
				msg_sent = MSG_SENT;
			}
        }
        break;
    }
    case ERROR: {
    	if ((!using_timer) &&( !timerRunning(timer))) {
    		using_timer = _USING;
			timerStart(timer, TIMER_MS2TICKS(TIMEOUT_ERROR), TIM_MODE_SINGLESHOT, callback_timer);
			turnOn_DebugLed_2();
		}
		else {
			if (timerExpired(timer)) {
				pre_state_fsm = state_fsm;
				state_fsm = IDLE;
				msg_sent = WAITING;
				turnOff_DebugLed_2();
				using_timer = _NOT_USING;
			}
			else if (!msg_sent) {
				print_msg("ERROR", MOVE);
				msg_sent = MSG_SENT;
			}
		}
        break;
    }
    case BRIGHT:{
    	if ((!using_timer) &&( !timerRunning(timer))) {
    		using_timer = _USING;
    		timerStart(timer, TIMER_MS2TICKS(TIMEOUT_BRIGHTNESS), TIM_MODE_SINGLESHOT, callback_timer);
    		turnOn_DebugLed_2();
		}
		else {
			if (timerExpired(timer)) {
				state_fsm = pre_state_fsm;
				pre_state_fsm = BRIGHT;
				msg_sent = WAITING;
				using_timer = _NOT_USING;
				turnOff_DebugLed_2();
			}
			if(encoder_inter == TURN){
				change_brightness(encoder.cantGiros);
				state_fsm = state_fsm;
			}
			else if(encoder_inter == BUTTON){
				state_fsm = pre_state_fsm;
				pre_state_fsm = BRIGHT;
				//msg_sent = WAITING;
				timerStop(timer);
				using_timer = _NOT_USING;
				turnOff_DebugLed_2();
				clearEncoderPresiones();
				clearEncoderGiros();
			}
		}
    	break;
    }
    	default: {
    		timerDelay(TIMER_MS2TICKS(5000));
			pre_state_fsm = IDLE;
			state_fsm = IDLE;
			msg_sent = WAITING;
			timerDelay(TIMER_MS2TICKS(5000));
			break;
    	}
    }
	if (!error)
		return 0;
	else
		return 1;
	}


/*******************************************************************************
 *  LOCAL FUNCTIONS DEFINITIONS
 ******************************************************************************/

//Función verify ID: tiene que chequear que el ID ingresado esté dentro de la base de datos
int verify_id(char data[]){
	int i, j, cant_equal = 0, user_num = -1;
	for(i = 0; i < NUM_USERS; i++){
		for(j = 0; j < NUM_ID; j++ ){
			if(data[j] != users[i].id[j]){
				j = NUM_ID;
				cant_equal = 0;
			}
			else{
				cant_equal++;
			}
		}
		if(cant_equal == NUM_ID){
			user_num = i;
			i = NUM_USERS;
		}
	}
	return user_num;
}

//Función verify ID: tiene que chequear que el ID ingresado esté dentro de la base de datos
int verify_pin(char data[], int user_num){
	int j;
	for(j = 0; j<NUM_PIN; j++){
		if (users[user_num].pin[j] != data[j]) {
			++users[user_num].attempts;
			return 0;
		}
	}
	return 1;
}

//Función que chequea si puedo agregar un nuevo usuario
int check_users(void) {
	int i, j, user_num = -1, cant_zeros = 0;
	for (i = 0; (i < 10) && (user_num == -1); i++) {
		for(j = 0; j < NUM_ID && user_num == -1; j++){
			if(users[i].id[j] == 0){
				++cant_zeros;
			}
		}
		if(cant_zeros == NUM_ID){
			user_num = i;
		}
		else{
			user_num = -1;
		}
	}
	return user_num;
}

//Función que agrega un nuevo usuario
void enter_new_user(user_t data, int user_num) {
	int i;
	for(i = 0; i<NUM_ID; i++){
		users[user_num].id[i] = data.id[i];
	}
	for(i = 0; i<NUM_PIN; i++){
		users[user_num].pin[i] = data.pin[i];
	}
	users[user_num].attribute = data.attribute;
	users[user_num].attempts = 0;
}

//Función que elimina un usuario
void remove_user(int user_num) {
	int i;
	users[user_num].attribute = 0;
	for(i = 0; i<NUM_ID; i++){
		users[user_num].id[i] = 0;
	}
	for(i = 0; i<NUM_PIN; i++){
		users[user_num].pin[i] = 0;
	}
	users[user_num].attempts = 0;
}

char get_number(void) {
	if (encoder.cantGiros >= 0) {
		if (encoder.cantGiros >= 10) {
			return (encoder.cantGiros % 10 + '0');
		}
		else {
			return (encoder.cantGiros + '0');
		}
	}
	else {
		if(!(encoder.cantGiros % 10)){
			return '0';
		}
		else if (encoder.cantGiros < -10) {
			return (10 + encoder.cantGiros % 10 + '0');
		}
		else {
			return (10 + encoder.cantGiros + '0');
		}
	}
}

void callback_timer(void) {

}

void change_password(char new_pass[], int user_count){
	int i;
	for(i = 0; i < NUM_PIN; i++){
		users[user_count].pin[i] = new_pass[i];
	}
}
