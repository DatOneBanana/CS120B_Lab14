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


static unsigned char _leader = 0;

enum leader_state { L_WAIT };
int tick_leader(int state) {
	_leader = !_leader;
    static unsigned char send_val = 0;

    switch (state) {
        case L_WAIT:
            if (_leader && USART_IsSendReady(0)) {
                send_val = !send_val;
                PORTA = send_val & 0x01;
                USART_Send(send_val, 0);
            }
	    break;
        default:
            state = L_WAIT;
	    break;
    }
    return state;
}

enum follower_state { F_WAIT };
int tick_follower(int state) {
    	_leader = !_leader;
	static unsigned char rec_val = 0;

    switch (state) {
        case F_WAIT:
            if (!_leader && USART_HasReceived(0)) {
		 rec_val = USART_Receive(0);
	         PORTA = rec_val & 0x01;
            }
	    break;
        default:
            state = F_WAIT;
	    break;
    }
    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0xFF; PORTA = 0x00;
    /* Insert your solution below */

	initUSART(0);

	static task task1, task2;
	task *tasks[] = { &task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);



	task1.state = 0;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &tick_leader;

	task2.state = 0;
	task2.period = 20;
	task2.elapsedTime = task2.period;
	task2.TickFct = &tick_follower;

	TimerSet(10);
	TimerOn();

    	unsigned short i;
    	while (1) {
		for(i = 0; i < numTasks; i++){
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 10;
		}
		while(!TimerFlag);
		TimerFlag = 0;
    	}

    return 1;
}
