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
   * error: variable que cuenta la cantidad de errores, si WRONG == 3 va al estado ERROR sino vuelve al estado de PRE lo que estaba antes
   * timeout: 1 si se excedió el tiempo de espera de un estado
   */

   /*******************************************************************************
    *  CONSTANT AND MACRO DEFINITIONS
    ******************************************************************************/
#define IDLE 0
#define PRE_ID 15
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
#define ERROR 1
#define BRIGHT 16

    //Definiciones de largos de datos
#define NUM_ID 19
#define NUM_PIN 4

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
#define ADMIN_ADD 2
#define ADMIN_REMOVE 3
#define ADMIN_BRIGHT 4
#define ADMIN_RESTART 5


/*******************************************************************************
 *  ENUMERATIONS, STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct {
    char id[19];
    char pin[4];
    int attribute;
    int attempts;
}user_t;

/*******************************************************************************
 *  VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
static user_t users[] = {
    {{'4','0', '4', '0','3' , '1', '0', '0' , '5', '1', '7' , '7', '5', '6' ,'4','8','0','0','0'},{'0','0','0','0'}, ADMIN_ATT, 0},
    {{0}, {0}, 0, 0},
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


void callback_timer(void);

//Me parece que voy a tener que agregar timers para cambiar de estado a veces o agregar estados intermedios
/*******************************************************************************
 *  GLOBAL FUNCTIONS DEFINITIONS
 ******************************************************************************/
int fsm(void) {
    static int user_count = 0, aux_user_count = 0, msg_sent = WAITING;
    static int i = 0;
    static int waiting_id = 0, waiting_pin = 0, error = 0;
    static char temp[] = "0";
    int encoder_inter = encoderInterrup();
    if (encoder_inter) {
        encoder = getEncoder();
    }
    bool magtek_inter = magtek_iter();
    static int pre_state_fsm = 0, state_fsm = 0;
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
                print_msg("Enter ID"); //---> FUNCION DEL DISPLAY.
                msg_sent = MSG_SENT;
            }
        }
        temp[0] = '0';
        break;
    }
    case PRE_ID: {
        //Estado en el que espero que ingresen todo el ID. Esto es si me mandan dígito por dígito.
        //Ver como me mandan los dígitos, si es que necesito un id_count para ver cuantos dígitos ingresaron o algo así.
    	if (!timerRunning(timer)) {
            timerStart(timer, TIMER_MS2TICKS(120000), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
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
						timerStop(timer);
						clearEncoderPresiones();
					}
					else if (waiting_id) {
                        pre_state_fsm = state_fsm;
                        if (encoder.cantPresion == OK) {
                            data.id[i] = get_number();
                            ++i;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                            temp[0] = '0';
                        }
                        else if (encoder.cantPresion == BACK) {
                            if (i == 0) {
                                pre_state_fsm = state_fsm;
                                i = 0;
                            }
                            else {
                                --i;
                            }
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }
                        else if (encoder.cantPresion == RESTART) {
                            i = 0;
                            pre_state_fsm = state_fsm;
                            state_fsm = IDLE;
                            msg_sent = WAITING;
                            timerStop(timer);
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }

                        if (i == NUM_ID) {
                            waiting_id = 0;
                            i = 0;
                            temp[0] = '0';
                        }
                    }
					if(encoder_inter == TURN){
						msg_sent = WAITING;
					}
					encoder_inter = clearEncoderInter();
					temp[0] = get_number();
                }
                if(!waiting_id) {
					pre_state_fsm = state_fsm;
					state_fsm = ID;
					msg_sent = WAITING;
					timerStop(timer);
				}
                else if(!msg_sent){
					print_msg(temp);
					msg_sent = MSG_SENT;
				}
            }
        }
        break;
    }
    case ID: {
        if (magtek_inter) {
            pre_state_fsm = state_fsm;
            magtek_inter = magtek_clear();
        }
        if (encoder_inter) {
        	pre_state_fsm = state_fsm;
        	if(encoder.cantPresion == BRIGHTNESS){
				state_fsm = BRIGHT;
				clearEncoderPresiones();
				clearEncoderGiros();
			}
			encoder_inter = clearEncoderInter();
			msg_sent = WAITING;
        }
        if(encoder.cantPresion != BRIGHTNESS){
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
        }
        break;
    }
    case ID_OK: {
        if (!timerRunning(timer)) {
            timerStart(timer, TIMER_MS2TICKS(20000), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
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
							clearEncoderPresiones();
						}
                    	else{
                            state_fsm = PRE_PIN;
                            waiting_pin = 1;
                            msg_sent = WAITING;
                            timerStop(timer);
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
                        print_msg("Enter PIN"); //---> FUNCION DEL DISPLAY.
                        msg_sent = MSG_SENT;
                    }
                }
            }
        }

        break;
    }
    case PRE_PIN: {
        if (!timerRunning(timer)) {
            timerStart(timer, TIMER_MS2TICKS(60000), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
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
					}
                	else if (waiting_pin) {
                		msg_sent = WAITING;
                        pre_state_fsm = state_fsm;
                        if (encoder.cantPresion == OK) {
                            data.pin[i] = get_number();
                            ++i;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                            temp[0] = '0';
                        }
                        else if (encoder.cantPresion == BACK) {
                            if (i == 0) {
                                pre_state_fsm = state_fsm;
                                i = 0;
                            }
                            else {
                                --i;
                            }
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }
                        else if (encoder.cantPresion == RESTART) {
                            i = 0;
                            pre_state_fsm = state_fsm;
                            state_fsm = IDLE;
                            msg_sent = WAITING;
                            timerStop(timer);
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }

                        if (i == NUM_PIN) {
							waiting_pin = 0;
							i = 0;
							temp[0] = '0';
							clearEncoderGiros();
							clearEncoderPresiones();
						}
                    }
                	if(encoder_inter == TURN){
						msg_sent = WAITING;
					}
                    encoder_inter = clearEncoderInter();
					temp[0] = get_number(); // ACA ES DONDE IMPRIME DONDE GIRA
                }
                if(!waiting_pin) {
                    pre_state_fsm = state_fsm;
                    state_fsm = PIN;
                    msg_sent = WAITING;
                    timerStop(timer);
                }
                else if(!msg_sent){
					print_msg(temp);
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
                state_fsm = OPEN;
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
    case OPEN: {
        if (!timerRunning(timer)) {
            timerStart(timer, TIMER_MS2TICKS(5000), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
            }
            else {
                if (magtek_inter || encoder_inter == BUTTON) {
                	if(encoder.cantPresion == BRIGHTNESS){
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						timerStop(timer);
					}
                	clearEncoderPresiones();
                	clearEncoderGiros();
                	encoder_inter = clearEncoderInter();
                	magtek_inter = magtek_clear();
                }

                if (!msg_sent) {
                    //Acá debería haber algo así como
                    print_msg("DOOR OPEN"); //---> FUNCION DEL DISPLAY.
                    msg_sent = MSG_SENT;
                }
            }
        }
        break;
    }
    case ADMIN: {
        if (!timerRunning(timer)) {
            timerStart(timer, TIMER_MS2TICKS(30000), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
            }
            else {
                if (magtek_inter) {
                    pre_state_fsm = state_fsm;
                    magtek_inter = 0;
                }
                if (encoder_inter) {
                    if (encoder.cantPresion == ADMIN_OPEN) {
                        pre_state_fsm = state_fsm;
                        state_fsm = OPEN;
                        msg_sent = WAITING;
                        timerStop(timer);
                        clearEncoderPresiones();
                        clearEncoderGiros();
                    }
                    else if (encoder.cantPresion == ADMIN_ADD) {
                        pre_state_fsm = state_fsm;
                        state_fsm = ADD;
                        timerStop(timer);
                        msg_sent = WAITING;
                        clearEncoderPresiones();
                        clearEncoderGiros();
                    }
                    else if (encoder.cantPresion == ADMIN_REMOVE) {
                        pre_state_fsm = state_fsm;
                        state_fsm = REMOVE;
                        timerStop(timer);
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
						clearEncoderPresiones();
						clearEncoderGiros();
                    }
                    else if (encoder.cantPresion == ADMIN_BRIGHT) {
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
						clearEncoderPresiones();
						clearEncoderGiros();
					}
                    encoder_inter = clearEncoderInter();
                }
                else if(!msg_sent){
                	print_msg("1 to open 2 to add 3 to remove");
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
                print_msg("Enter ID");
                msg_sent = MSG_SENT;
            }
            waiting_id = 1;
        }
        else {
            pre_state_fsm = state_fsm;
            state_fsm = ERROR;
            msg_sent = WAITING;
        }
        break;
    }
    case NEW_ID: {
        if (!timerRunning(timer)) {
            timerStart(timer, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
            }
            else if (magtek_inter){
            	pre_state_fsm = state_fsm;
				state_fsm = NEW_PIN;
				ptr = get_ID();
				for (int i = 0; i < NUM_ID; i++) {
					data.id[i] = *ptr;
					++ptr;
				}
				magtek_inter = magtek_clear();
				waiting_id = 0;
            }
            else if(encoder_inter) {
                pre_state_fsm = state_fsm;
                if (encoder.cantPresion == BRIGHTNESS) {
					pre_state_fsm = state_fsm;
					state_fsm = BRIGHT;
					msg_sent = WAITING;
					timerStop(timer);
					clearEncoderPresiones();
				}
                else if (waiting_id) {
                    if (encoder.cantPresion == OK) {
                        data.id[i] = get_number();
                        ++i;
                        clearEncoderPresiones();
                        clearEncoderGiros();
                        temp[0] = '0';
                    }
                    else if (encoder.cantPresion == BACK) {
                        if (i == 0) {
                            pre_state_fsm = state_fsm;
                            i = 0;
                        }
                        else {
                            --i;
                        }
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

                    if (i == NUM_ID) {
                        waiting_id = 0;
                        i = 0;
                        temp[0] = '0';
                    }
                }
                if(encoder_inter == TURN){
					msg_sent = WAITING;
				}
                encoder_inter = clearEncoderInter();
                temp[0] = get_number();
            }

            if(!waiting_id){
            	//Fijarse si entra aca entrando con la magtek
            	state_fsm = NEW_PIN;
				msg_sent = WAITING;
				if (!msg_sent) {
					print_msg("Enter PIN");
					msg_sent = MSG_SENT;
				}
				waiting_pin = 1;
            }
            else if(!msg_sent){
            	msg_sent = MSG_SENT;
            	print_msg(temp);
            }
        }
        break;
    }
    case NEW_PIN: {
        if (!timerRunning(timer)) {
            timerStart(timer, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
            }
            else {
                if (magtek_inter || encoder_inter) {
                    magtek_inter = magtek_clear();
                    pre_state_fsm = state_fsm;
                    if (encoder.cantPresion == BRIGHTNESS) {
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
						clearEncoderPresiones();
					}
                    else if (waiting_pin) {
                        pre_state_fsm = state_fsm;
                        if (encoder.cantPresion == OK) {
                            data.pin[i] = get_number();
                            ++i;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                            temp[0] = '0';
                        }
                        else if (encoder.cantPresion == BACK) {
                            if (i == 0) {
                                pre_state_fsm = state_fsm;
                                i = 0;
                            }
                            else {
                                --i;
                            }
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }
                        else if (encoder.cantPresion == RESTART) {
                            i = 0;
                            pre_state_fsm = state_fsm;
                            state_fsm = IDLE;
                            msg_sent = WAITING;
                            timerStop(timer);
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }

                        if(encoder_inter == TURN){
                        	msg_sent = WAITING;
                        }

                        if (i == NUM_PIN) {
                            waiting_pin = 0;
                            i = 0;
                            temp[0] = '0';
                        }

                    }
                    encoder_inter = clearEncoderInter();
                    temp[0] = get_number();
                }
                if(!waiting_pin){
                	state_fsm = NEW_ADMIN;
					msg_sent = WAITING;
					if (!msg_sent) {
						print_msg("Admin?");
						msg_sent = MSG_SENT;
					}
				}
				else if(!msg_sent){
					msg_sent = MSG_SENT;
					print_msg(temp);
				}
            }
        }
        break;
    }
    case NEW_ADMIN: {
        if (!timerRunning(timer)) {
            timerStart(timer, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
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
                    }
                    else if (encoder.cantPresion == 2) {
                        data.attribute = NOT_ADMIN_ATT;
                        enter_new_user(data, aux_user_count);
                        pre_state_fsm = state_fsm;
                        state_fsm = ADMIN;
                        msg_sent = WAITING;
                        timerStop(timer);
                    }
                    else if (encoder.cantPresion == RESTART) {
                        i = 0;
                        pre_state_fsm = state_fsm;
                        state_fsm = IDLE;
                        msg_sent = WAITING;
                        timerStop(timer);
                    }
                    else if (encoder.cantPresion == BRIGHTNESS) {
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
					}
                    clearEncoderPresiones();
                    clearEncoderGiros();
                    clearEncoderInter();
                }

                if(!msg_sent){
					print_msg("1 if admin 2 if not");
					msg_sent = MSG_SENT;
				}
                else {
                    pre_state_fsm = state_fsm;
                }
            }
        }
        break;
    }
    case REMOVE: {
        if (!timerRunning(timer)) {
            timerStart(timer, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, callback_timer);
        }
        else {
            if (timerExpired(timer)) {
                pre_state_fsm = state_fsm;
                state_fsm = IDLE;
                user_count = 0;
                msg_sent = WAITING;
            }
            else {
                if (encoder_inter || magtek_inter) {
                    magtek_inter = magtek_clear();
                    if (encoder.cantPresion == BRIGHTNESS) {
						pre_state_fsm = state_fsm;
						state_fsm = BRIGHT;
						msg_sent = WAITING;
						timerStop(timer);
						clearEncoderPresiones();
					}
                    else if (waiting_id) {
                        pre_state_fsm = state_fsm;
                        if (encoder.cantPresion == OK) {
                            aux_user_count = get_number()-'0';
                            ++i;
                            clearEncoderPresiones();
                            clearEncoderGiros();
                            temp[0] = '0';
                        }
                        else if (encoder.cantPresion == BACK) {
                            if (i == 0) {
                                pre_state_fsm = state_fsm;
                                i = 0;
                            }
                            else {
                                --i;
                            }
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }
                        else if (encoder.cantPresion == RESTART) {
                            i = 0;
                            pre_state_fsm = state_fsm;
                            state_fsm = IDLE;
                            msg_sent = WAITING;
                            timerStop(timer);
                            clearEncoderPresiones();
                            clearEncoderGiros();
                        }

                        if (i == 1) {
                            waiting_id = 0;
                            i = 0;
                            temp[0] = '0';
                        }
                    }
                    if(!waiting_id){
						state_fsm = REMOVE_ID;
						pre_state_fsm = state_fsm;
						msg_sent = WAITING;
					}
					else if(!msg_sent){
						msg_sent = MSG_SENT;
						print_msg(temp);
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
        if (!msg_sent) {
            print_msg("Timeout");
            msg_sent = MSG_SENT;
        }
        else{
            pre_state_fsm = state_fsm;
            state_fsm = IDLE;
            msg_sent = WAITING;
        }
        break;
    }
    case ERROR: {
        if (!msg_sent) {
            print_msg("ERROR");
        }
		else{
			pre_state_fsm = state_fsm;
			state_fsm = IDLE;
			msg_sent = WAITING;
		}
        break;
    }
    case BRIGHT:{
    	if (!timerRunning(timer)) {
    		timerStart(timer, TIMER_MS2TICKS(15000), TIM_MODE_SINGLESHOT, callback_timer);
		}
		else {
			if (timerExpired(timer)) {
				state_fsm = pre_state_fsm;
				pre_state_fsm = BRIGHT;
				msg_sent = WAITING;
			}
			if(encoder_inter == TURN){
				change_brightness(encoder.cantGiros);
				state_fsm = state_fsm;
			}
			else if(encoder_inter == BUTTON){
				state_fsm = pre_state_fsm;
				pre_state_fsm = BRIGHT;
				msg_sent = WAITING;
				timerStop(timer);
				clearEncoderPresiones();
				clearEncoderGiros();
			}
		}
    }
    default: {
        pre_state_fsm = IDLE;
        state_fsm = IDLE;
        msg_sent = WAITING;
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
	int i = 0, j = 0;
	int user_num = 0;
	for (i = 0; i < 10; i++) {
		//Ver bien como son las estructuras!!
		for(j = 0; j<NUM_ID; j++){
			if (users[i].id[j] != data[j]) { //*data
				user_num = -1;
				j = NUM_ID;
			}
		}
		if(user_num != -1){
			user_num = i;
			i = 10;
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
	int i, j, user_num = -1;
	for (i = 0; (i < 10) && (user_num == -1); i++) {
		for(j = 0; j < NUM_ID && user_num == -1; j++){
			if(users[i].id[j] != 0){
				user_num = i;
			}
		}
		if(user_num != -1){
			for(j = 0; (j < NUM_PIN) && (user_num == -1); j++){
				if(users[i].pin[j] != 0){
					user_num = i;
				}
			}
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
		if (encoder.cantGiros <= -10) {
			return (10 + encoder.cantGiros % 10 + '0');
		}
		else {
			return (10 + encoder.cantGiros + '0');
		}
	}
}

void callback_timer(void) {

}