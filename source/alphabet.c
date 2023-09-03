/*
 * alphabet.c
 *
 *  Created on: 31 ago. 2023
 *      Author: Guada Voss
 */
#include "alphabet.h"

uint8_t translateChar(char letter) {

	uint8_t c = 0;
	switch(letter) {
		case 'a':
			c = A;
			break;
		case 'A':
			c = A;
			break;
		case 'b':
			c = B;
			break;
		case 'B':
			c = B;
			break;
		case 'c':
			c = C;
			break;
		case 'C':
			c = C;
			break;
		case 'd':
			c = D;
			break;
		case 'D':
			c = D;
			break;
		case 'e':
			c = E;
			break;
		case 'E':
			c = E;
			break;
		case 'f':
			c = F;
			break;
		case 'F':
			c = F;
			break;
		case 'g':
			c = G;
			break;
		case 'G':
			c = G;
			break;
		case 'h':
			c = H;
			break;
		case 'H':
			c = H;
			break;
		case 'i':
			c = I;
			break;
		case 'I':
			c = I;
			break;
		case 'j':
			c = J;
			break;
		case 'J':
			c = J;
			break;
		case 'k':
			c = K;
			break;
		case 'K':
			c = K;
			break;
		case 'l':
			c = L;
			break;
		case 'L':
			c = L;
			break;
		case 'm':
			c = M;
			break;
		case 'M':
			c = M;
			break;
		case 'n':
			c = N;
			break;
		case 'N':
			c = N;
			break;
		case 'o':
			c = O;
			break;
		case 'O':
			c = O;
			break;
		case 'p':
			c = P;
			break;
		case 'P':
			c = P;
			break;
		case 'q':
			c = Q;
			break;
		case 'Q':
			c = Q;
			break;
		case 'r':
			c = R;
			break;
		case 'R':
			c = R;
			break;
		case 's':
			c = S;
			break;
		case 'S':
			c = S;
			break;
		case 't':
			c = T;
			break;
		case 'T':
			c = T;
			break;
		case 'u':
			c = U;
			break;
		case 'U':
			c = U;
			break;
		case 'v':
			c = V;
			break;
		case 'V':
			c = V;
			break;
		case 'w':
			c = W;
			break;
		case 'W':
			c = W;
			break;
		case 'x':
			c = X;
			break;
		case 'X':
			c = X;
			break;
		case 'y':
			c = Y;
			break;
		case 'Y':
			c = Y;
			break;
		case 'z':
			c = Z;
			break;
		case 'Z':
			c = Z;
			break;
		case (' '):
			c = 0;
			break;
		case ('0'):
			c = NUM_0;
			break;
		case ('1'):
			c = NUM_1;
			break;
		case ('2'):
			c = NUM_2;
			break;
		case ('3'):
			c = NUM_3;
			break;
		case ('4'):
			c = NUM_4;
			break;
		case ('5'):
			c = NUM_5;
			break;
		case ('6'):
			c = NUM_6;
			break;
		case ('7'):
			c = NUM_7;
			break;
		case ('8'):
			c = NUM_8;
			break;
		case ('9'):
			c = NUM_9;
			break;
		case ('?'):
			c = PREG;
			break;
		case ('-'):
			c = MENOS;
			break;
		default:
			c = 0;

	}

	return c;
}


