/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "scheduler.h"
#include "usart_ATmega1284.h"
#include "timer.h"
#endif

static unsigned char _leader = 1;

enum leader_state { U_WAIT };
int tick_leader(int state) {
    static unsigned char send_val = 0;

    switch (state) {
        case U_WAIT:
            if (_leader && USART_IsSendReady(0)) {
                send_val = !send_val;
                PORTA = send_val & 0x01;
                USART_Send(send_val, 0);
            }
        default:
            state = U_WAIT;
    }
    return state;
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0xFF; PORTA = 0x00;
    /* Insert your solution below */

	initUSART(0);

	static task task1;
	task *tasks[] = { &task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);



	task1.state = -1;
	task1.period = 1000;
	task1.elapsedTime = task1.period;
	task1.TickFct = &tick_leader;

	TimerSet(1000);
	TimerOn();

    	unsigned short i;
    	while (1) {
		for(i = 0; i < numTasks; i++){
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1000;
		}
		while(!TimerFlag);
		TimerFlag = 0;
    	}

    return 1;
}
