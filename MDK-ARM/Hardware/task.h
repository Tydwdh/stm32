/**
 * @file task.h
 * @brief 单片机裸机前后台系统的简单实现
 * @note 移植须知：将time_counter++放入一个间隔固定的定时器中作为任务系统时钟。
 * 1. 定义task_t结构体，包含任务函数指针和任务周期
 * 2. 定义Task_Create函数，创建任务
 * 3. 调用Task_Scheduler实现任务调度
 * @version 0.1
 * @date 2024-09-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef _TASK_H_
#define _TASK_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"

#define MAX_TASKS 20

extern volatile uint32_t time_counter;



typedef struct task_init_t
{
	void (*task_func)(void);
	uint32_t period;
} task_init_t;


typedef enum
{
	TASK_RUNNING ,
	TASK_SUSPENDED,
	TASK_DELETED ,
} task_state_t;

typedef struct task_t
{
	task_init_t init;


	uint32_t last_time;

	task_state_t state;

	int16_t next; // 指向下一个任务的索引
} task_t;



int16_t Task_Create(task_t * task);
void Task_Delete(task_t * task);
void Task_Suspend(task_t * task);
void Task_Resume(task_t * task);
void Task_Scheduler(void);

#endif /* _TASK_H_ */
