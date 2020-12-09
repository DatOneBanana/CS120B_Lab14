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

static unsigned char leaderSignal = 0;

enum leader_state { L_WAIT};
int tick_leader(int state) {
    static unsigned char send_val = 0;

    switch (state) {
        case L_WAIT:
            if (leaderSignal && USART_IsSendReady(1)) {
                send_val = !send_val;
                PORTA = send_val & 0x01;
                USART_Send(send_val, 1);
            }
        default:
            state = L_WAIT;
    }
    return state;
}
enum follower_states{F_WAIT};
int tick_follower(int state) {
    static unsigned char rec_val = 0;

    switch (state) {
        case F_WAIT:
            if (!leaderSignal && USART_HasReceived(0)) {
		rec_val = USART_Receive(0);
                PORTA = rec_val & 0x01;
            }
        default:
            state = F_WAIT;
    }
    return state;
}
enum button_states{U_WAIT, U_WAIT2};
int tick_button(int state) {
   
	 switch (state) {
		case U_WAIT:
			if(~PINB & 0x01)
				state = U_WAIT2;
				break;
        	case U_WAIT2:
            		if (~PINB & 0x01) {
				state = U_WAIT2;
            		}
			else
			{
				leaderSignal = !leaderSignal;
				PORTC = leaderSignal;
				state = U_WAIT;
			}
			break;
        	default:
            		state = U_WAIT;
    }
    return state;
}

int main(void) {
        /* Insert DDR and PORT initializations */
        DDRA = 0xFF; PORTA = 0x00;
        DDRB = 0x00; PORTB = 0xFF;
        DDRC = 0xFF; PORTC = 0x00;

        initUSART(0);
        initUSART(1);

        static task task1, task2, task3;
        task *tasks[] = { &task1, &task2, &task3};
        const unsigned short numTasks = sizeof(tasks)/sizeof(task*);


        task1.state = -1;
        task1.period = 1000;
        task1.elapsedTime = task1.period;
        task1.TickFct = &tick_leader;

        task2.state = -1;
        task2.period = 20;
        task2.elapsedTime = task2.period;
        task2.TickFct = &tick_follower;

        task3.state = -1;
        task3.period = 20;
        task3.elapsedTime = task3.period;
        task3.TickFct = &tick_button;

        TimerSet(20);
        TimerOn();

        unsigned short i;
        while (1) {
                for(i = 0; i < numTasks; i++){
                        if(tasks[i]->elapsedTime == tasks[i]->period){
                                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                                tasks[i]->elapsedTime = 0;
                        }
                        tasks[i]->elapsedTime += 20;
                }
                while(!TimerFlag);
                TimerFlag = 0;
        }

    return 1;                                                                                                                                                    
}
