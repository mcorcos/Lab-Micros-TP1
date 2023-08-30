/***************************************************************************//**
  @file     FSM.c
  @brief    State Machine
  @author   Grupo 4 Laboratorio de Microprocesadores:
            Corcos, Manuel
            Lesiuk, Alejandro
            Paget, Milagros
            Voss, Maria de Guadalupe
  ******************************************************************************/

  /*******************************************************************************
   * INCLUDE HEADER FILES
   ******************************************************************************/

#include "FSM.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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

//A continuación defino los tiempos máximos que puedo permanecer en un estado sin interrupciones.
//Pensar si esto tiene q estar acá o que cada uno de los chicos se fije :)
#define MAX_PRE_ID 700 //invente este numero pero hay q pensarlo
#define MAX_ID 800
#define MAX_ID_OK 900
#define MAX_PRE_PIN 700
#define MAX_ADMIN 800
#define MAX_NEW_ID 800
#define MAX_NEW_PIN 800
#define MAX_NEW_ADMIN 800
#define MAX_REMOVE_ID 800

//Definiciones de acciones con el encoder
#define OK 0
#define BACK 1
#define RESTART 2


//Posibles atributos de los usuarios
#define NOT_ADMIN_ATT 0
#define ADMIN_ATT 1

#define ADMIN_OPEN 0
#define ADMIN_ADD 1
#define ADMIN_REMOVE 2

/*COSAS A DEFINIR CON LOS CHICOS :
- PIPI:
    1. print_msg()
- ALITO:
    1. del encoder me mandan digito por digito o me mandan el numero entero? Para ver como guardar los datos.
    2. estado PRE_ID
- MANU:
    1. get_id() 
- TODOS:
    1. No se bien como debería funcionar esto, siento que yo debería tener una función al principio de fsm que sea tipo 
    update_interruptions();
    que le pida a todos que me digan si hubo o no una interrupcion 
    2. Definir qué hacemos con las diferentes maneras de interaccionar con el encoder
*/

/*******************************************************************************
 *  ENUMERATIONS, STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct{
    int id;
}id_t;

typedef struct{
    int pin;
}pin_t;

typedef struct{
    id_t id;
    pin_t pin;
    int attribute;
    int attempts;
}user_t;


/*******************************************************************************
 *  VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
user_t users[] = {
    {123456, 1233, ADMIN, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL}
};



/*******************************************************************************
 *  FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

 /**
  * @brief Verifies that the entered ID exists in the database.
  * @param The ID that needs to be verified.
  * @return number of id in the array or -1 if it does not exist.
  */
int verify_id(id_t data);

/**
 * @brief Verifies that the entered PIN is correct for the user being used.
 * @param data is the PIN that needs to be verified.
 * @param user_num is the order in the array of this user.
 * @return 1 if it is correct or 0 if it is not.
 */
int verify_pin(pin_t data, int user_num);

/**
 * @brief Starts a 5 second count.
 * @param mode: 1 to start, 0 to update.
 * @return If timer ended returns 1.
 */
int begin_5seg(int mode = 0);

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

/*AGREGADO PARA PROBAR!!!*/
void print_msg(const char* msg);

//encoder_inter y magtek_inter deberian ser globales o algo asi!!!
//encoder_inter podria tener valores dependiendo de que accion tengo que hacer

//si estan poniendo el pin entonces que ignore todo lo de la tarjeta

/*******************************************************************************
 *  GLOBAL FUNCTIONS DEFINITIONS
 ******************************************************************************/
int fsm(void) {
    static int user_count = 0, timer_5seg = 0, time_is_up = 0, aux_user_count = 0;
    int admin_move = 0;
    static int waiting_id = 0, waiting_pin = 0, error = 0, encoder_inter = 0, magtek_inter = 0;
    static int pre_state = 0, state = 0;
    static int counter = 0; //counter = 0 siempre que cambio de estado.
    static user_t data; //Variable que almacena la información que entra por el encoder (?)
    
    /*Inicialización de la variable data*/
    data.attempts = NULL;
    data.attribute = NULL;
    data.id.id = NULL;
    data.pin.pin = NULL;
    
    switch (state) {
        case IDLE: {
            if (encoder_inter && !magtek_inter) {
                //Si hubo una interacción con el encoder quiere decir que me ingresan el ID por el encoder.
                //Paso al estado PRE_ID y reinicio el contador.
                pre_state = state;
                state = PRE_ID;
                counter = 0;
                waiting_id = 1;
                encoder_inter = 0;
            }
            else if (magtek_inter && !encoder_inter) {
                //Si hubo una interacción con la magtek quiere decir que me ingresan el ID con tarjeta.
                //Paso al estado ID y reinicio el contador.
                pre_state = state;
                state = ID;
                //data = get_id(); ----> FUNCIÓN MAGTEK q me de el ID de la tarjeta!
                counter = 0;
                magtek_inter = 0;
            }
            else if (magtek_inter && encoder_inter) {
                //No debería haber dos interacciones a la vez.
                //Paso al estado ERROR y reinicio el contador.
                pre_state = state;
                state = ERROR;
                counter = 0;
                magtek_inter = 0;
                encoder_inter = 0;
            }
            else {
                pre_state = state;
                state = state;
                counter++;
                //Acá debería haber algo así como 
                print_msg("Enter ID"); //---> FUNCION DEL DISPLAY.
                /*AGREGADO PARA PROBAR!!!!*/
                if (counter == 8)
                    error = 1;
                else
                    error = 0;
            }
            break;
        }
             //Charlar con alito!!!
        case PRE_ID: {
        //Estado en el que espero que ingresen todo el ID. Esto es si me mandan dígito por dígito.
        //Ver como me mandan los dígitos, si es que necesito un id_count para ver cuantos dígitos ingresaron o algo así.
            if (counter == MAX_PRE_ID) {
                pre_state = state;
                state = TIMEOUT;
                counter = 0;
            }
            else if (magtek_inter) {
                pre_state = state;
                magtek_inter = 0;
            }
            else {
                if (waiting_id) {
                    pre_state = state;
                    //No se si lo de data tiene q ir aca o en el else de abajo :) depende como me mande la info
                    counter++;
                }
                else {
                    pre_state = state;
                    state = ID;
                    counter = 0;
                }
            }
            break;
        }
        case ID: {
            if (counter == MAX_ID) {
                pre_state = state;
                state = TIMEOUT;
                counter = 0;
            }
            else if (magtek_inter) {
                pre_state = state;
                magtek_inter = 0;
            }
            //Charlar con los chicos para definir que onda
            else if (encoder_inter) {
                //Acá ver a que estado tengo que pasar dependiendo de lo que hacen con el encoder.
                //switch (encoder_inter)
                //{
                //}
                encoder_inter = 0;
            }
            else {
                user_count = verify_id(data.id);
                if(user_count!= -1){
                    pre_state = state;
                    state = ID_OK;
                    counter = 0;
                }
                else {
                    pre_state = state;
                    state = ERROR;
                    counter = 0;
                    user_count = 0;
                }
            }
            break;
            }
        case ID_OK: {
            if (counter == MAX_ID_OK) {
                pre_state = state;
                state = TIMEOUT;
                counter = 0;
            }
            else {
                if (encoder_inter && !magtek_inter) {
                    //Si hubo una interacción con el encoder quiere decir que me ingresan el ID por el encoder.
                    //Paso al estado PRE_ID y reinicio el contador.
                    pre_state = state;
                    state = PRE_PIN;
                    counter = 0;
                    waiting_pin = 1;
                    encoder_inter = 0;
                }
                else if (magtek_inter && !encoder_inter) {
                    //Si hubo una interacción con la magtek quiere decir que me ingresan el ID con tarjeta.
                    //Paso al estado ID y reinicio el contador.
                    pre_state = state;
                    state = ERROR;
                    counter = 0;
                    magtek_inter = 0;
                }
                else if (magtek_inter && encoder_inter) {
                    pre_state = state;
                    state = ERROR;
                    counter = 0;
                    magtek_inter = 0;
                    encoder_inter = 0;
                }
                else {
                    pre_state = state;
                    state = state;
                    counter++;
                    //Acá debería haber algo así como 
                    print_msg("Enter PIN"); //---> FUNCION DEL DISPLAY.
                }
            }
            break;
        }
        case PRE_PIN: {
            if (counter == MAX_PRE_PIN) {
                pre_state = state;
                state = TIMEOUT;
                counter = 0;
            }
            else if (magtek_inter) {
                pre_state = state;
                state = ERROR;
                counter = 0;
                magtek_inter = 0;
            }
            else {
                if (waiting_pin) {
                    pre_state = state;
                    //No se si lo de data tiene q ir aca o en el else de abajo :) depende como me mande la info
                    counter++;
                }
                else {
                    pre_state = state;
                    state = ID;
                    counter = 0;
                }
            }
            break;
        }
        case PIN: {
            if (verify_pin(data.pin, user_count)) {
                pre_state = state;
                users[user_count].attempts = 0;
                if (users[user_count].attribute) {
                    state = ADMIN;
                    counter = 0;
                }
                else {
                    state = OPEN;
                    counter = 0; 
                }
            }
            else {
                if (users[user_count].attempts >= 3) {
                    remove_user(user_count);
                    pre_state = state;
                    state = ERROR;
                    counter = 0;
                    user_count = 0;
                }
                else {
                    pre_state = state;
                    state = ID_OK;
                    counter = 0;
                }
            }
            break;
        }
        case OPEN: {
            //Como hago el timer de 5 seg
            if (magtek_inter || encoder_inter) {
                state = state;
                pre_state = state;
            }
            if (timer_5seg) {
                if (time_is_up = begin_5seg()) {
                    pre_state = state;
                    state = IDLE;
                    user_count = 0;
                    timer_5seg = 0;
                    //Acá debería haber algo así como 
                    print_msg("DOOR OPEN"); //---> FUNCION DEL DISPLAY.
                }
            }
            else {
                begin_5seg(1);
                timer_5seg = 1;
            }
            break;
        }
        case ADMIN:{
            if (counter == MAX_ADMIN) {
                pre_state = state;
                state = TIMEOUT;
                counter = 0;
            }
            else {
                if (encoder_inter) {
                    if (admin_move == ADMIN_OPEN) {
                        pre_state = state;
                        state = OPEN;
                        counter = 0;
                    }
                    else if (admin_move == ADMIN_ADD) {
                        pre_state = state;
                        state = ADD;
                        counter = 0;
                    }
                    else if (admin_move == ADMIN_REMOVE) {
                        pre_state = state;
                        state = REMOVE;
                        counter = 0;
                    }
                }
            }
            break;
        }        
        case ADD: {
            aux_user_count = check_users();
            if (aux_user_count != -1) {
                pre_state = state;
                state = NEW_ID;
                counter = 0;
                print_msg("Enter ID");
            }
            else {
                pre_state = state;
                state = ERROR;
                counter = 0;
            }
            break;
        }
        case NEW_ID: {
            if (counter == MAX_NEW_ID) {
                pre_state = state;
                state = TIMEOUT;
                counter = 0;
            }
            else if (magtek_inter || encoder_inter) {
                //data.id = get_id();
                pre_state = state;
                state = NEW_PIN;
                counter = 0;
                print_msg("Enter PIN");
            }
            else {
                pre_state = state;
                counter++;
            }
            break;
        }
        case NEW_PIN: {
            if (counter == MAX_NEW_PIN) {
                pre_state = state;
                state = TIMEOUT;
                counter = 0;
            }
            else if (encoder_inter) {
                //data.pin = get_pin();
                pre_state = state;
                state = NEW_ADMIN;
                counter = 0;
                print_msg("Admin?");
            }
            else {
                pre_state = state;
                counter++;
            }
            break;
        }
        case NEW_ADMIN: {
            if (counter == MAX_NEW_ADMIN) {
                pre_state = state;
                state = TIMEOUT;
                counter = 0;
            }
            else if (encoder_inter) {
                //data.attribute = get_attribute();
                enter_new_user(data, aux_user_count);
                pre_state = state;
                state = ADMIN;
                counter = 0;
            }
            else {
                pre_state = state;
                counter++;
            }
            break;
        }
        case REMOVE: {
            if (counter == MAX_REMOVE_ID) {
                pre_state = state;
                state = TIMEOUT;
                counter = 0;
            }
            else if (encoder_inter) {
                //data.id = get_id();
                pre_state = state;
                state = REMOVE_ID;
                counter = 0;
            }
            else {
                pre_state = state;
                state = ERROR;
                counter = 0;
            }
            break;
        }
        case REMOVE_ID: {
            aux_user_count = verify_id(data.id);
            if (aux_user_count) {
                remove_user(aux_user_count);
                counter = 0;
                aux_user_count = 0;
                pre_state = state;
                state = ADMIN;
                counter = 0;
            }
            else {
                pre_state = state;
                state = ERROR;
                counter = 0;
            }
            break;
        }
        case TIMEOUT: {
            print_msg("Timeout");
            pre_state = state;
            state = IDLE;
            break;
        }
        case ERROR: {
            print_msg("ERROR");
            pre_state = state;
            state = IDLE;
            break;
        }
        default: {
            pre_state = IDLE;
            state = IDLE;
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
int verify_id(id_t data){
    int i = 0;
    int user_num = 0;
    for (i = 0; i < 10; i++) {
        //Ver bien como son las estructuras!!
        if (users[i].id.id == data.id) {
            user_num = i;
            i = 10;
        }
        else {
            user_num = -1;
        }
    }
    return user_num;
}

//Función verify ID: tiene que chequear que el ID ingresado esté dentro de la base de datos
int verify_pin(pin_t data, int user_num) {
    if (data.pin == users[user_num].pin.pin) {
        return 1;
    }
    else {
        ++users[user_num].attempts;
        return 0;
    }
}

//Función que empieza el conteo de los 5 segundos del timer
int begin_5seg(int mode) {
    return 0;
}

//Función que chequea si puedo agregar un nuevo usuario
int check_users(void) {
    int i, user_num = 0;
    for (i = 0; i < 10; i++) {
        //Ver bien esto desp :)
        if (users[i].id.id == NULL) {
            if (users[i].pin.pin == NULL){
                user_num = i;
                i = 10;
            }
        }
        else {
            user_num = -1;
            ++i;
        }
    }
    return user_num;
}

//Función que agrega un nuevo usuario
void enter_new_user(user_t data, int user_num) {
    users[user_num].id = data.id;
    users[user_num].pin = data.pin;
    users[user_num].attribute = data.attribute;
    users[user_num].attempts = NULL;
}

//Función que elimina un usuario
void remove_user(int user_num) {
    users[user_num].attribute = NULL;
    users[user_num].id.id = NULL;
    users[user_num].pin.pin = NULL;
    users[user_num].attempts = NULL;
}


/*AGREGADO PARA PROBAR!!!!*/
void print_msg(const char* msg) {
    printf(msg);
}