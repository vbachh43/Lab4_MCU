/*
 * scheduler.c
 *
 *  Created on: Nov 29, 2023
 *      Author: Admin
 */

#include "scheduler.h"
#include "main.h"

//extern TIM_HandleTypeDef htim2;

sTask SCH_Tasks_G[SCH_MAX_TASKS];
uint8_t current_index_task = 0;

void SCH_Init(void){
	current_index_task = 0;
}

void SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
//	uint32_t tick = htim2.Init.Period;
	if (current_index_task < SCH_MAX_TASKS){
		SCH_Tasks_G[current_index_task].pTask = pFunction;
		SCH_Tasks_G[current_index_task].Delay = DELAY;
		SCH_Tasks_G[current_index_task].Period = PERIOD;
		SCH_Tasks_G[current_index_task].RunMe = 0;

		SCH_Tasks_G[current_index_task].TaskID = current_index_task;

		current_index_task++;
	}
}

void SCH_Update(void){
	for (int i = 0; i <  current_index_task; i ++){
		if (SCH_Tasks_G[i].Delay > 0){
			SCH_Tasks_G[i].Delay--;
		}
		else{
			SCH_Tasks_G[i].Delay = SCH_Tasks_G[i].Period;
			SCH_Tasks_G[i].RunMe++;
		}
	}
}

void SCH_Dispatch_Tasks(void){
	for (int i = 0; i < current_index_task; i++){
		if (SCH_Tasks_G[i].RunMe > 0){
			SCH_Tasks_G[i].RunMe--;
			(*SCH_Tasks_G[i].pTask)();

			if (SCH_Tasks_G[i].Period == 0){
				SCH_Delete(i);
			}
		}
	}
}

void SCH_Delete(uint32_t ID){
	if (ID < 0 || ID > current_index_task){
		return;
	}
//	else if (ID == current_index_task){
//		SCH_Tasks_G[current_index_task].pTask = 0x0000;
//		SCH_Tasks_G[current_index_task].Delay = 0;
//		SCH_Tasks_G[current_index_task].Period = 0;
//		SCH_Tasks_G[current_index_task].RunMe = 0;
//
//		SCH_Tasks_G[current_index_task].TaskID = 0;
//	}
	else if (ID < current_index_task){
		for (int i = ID; i < current_index_task; i++){
			SCH_Tasks_G[i].pTask = SCH_Tasks_G[i + 1].pTask;
			SCH_Tasks_G[i].Delay = SCH_Tasks_G[i + 1].Delay;
			SCH_Tasks_G[i].Period = SCH_Tasks_G[i + 1].Period;
			SCH_Tasks_G[i].RunMe = SCH_Tasks_G[i + 1].RunMe;

			SCH_Tasks_G[i].TaskID = SCH_Tasks_G[i+1].TaskID;
		}
		SCH_Tasks_G[current_index_task].pTask = 0x0000;
		SCH_Tasks_G[current_index_task].Delay = 0;
		SCH_Tasks_G[current_index_task].Period = 0;
		SCH_Tasks_G[current_index_task].RunMe = 0;

		SCH_Tasks_G[current_index_task].TaskID = 0;

		current_index_task--;
	}
}

void SCH_Report_Status ( void ) {
# ifdef SCH_REPORT_ERRORS
// ONLY APPLIES IF WE ARE REPORTING ERRORS
// Check for a new error code
  if ( Error_code_G != Last_error_code_G ) {
	// Negative l o gic on LEDs assumed
	Error_port = 255 − Error_code_G ;
	Last_error_code_G = Error_code_G ;
	if (Error_code_G != 0){
		Error_tick_count_G = 60000;
	}
	else{
		Error_tick_count_G = 0;
	}
  }
  else {
	if ( Error_tick_count_G != 0 ) {
		if (−−Error_tick_count_G == 0 ) {
			Error_code_G = 0; // Reset e r ro r code
		}
		}
	}
	#endif
}
