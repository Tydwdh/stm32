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

#define MAX_TASKS 20 // 最大任务数量

extern volatile uint32_t time_counter; // 时间计数器

/**
 * @brief 任务初始化结构体
 */
typedef struct task_init_t
{
	void (*task_func)(void); // 任务函数指针
	uint32_t period; // 任务周期
} task_init_t;

/**
 * @brief 任务状态枚举
 */
typedef enum
{
	TASK_RUNNING, // 任务运行中
	TASK_SUSPENDED, // 任务挂起
	TASK_DELETED, // 任务已删除
} task_state_t;

/**
 * @brief 任务结构体
 */
typedef struct task_t
{
	task_init_t init; // 任务初始化结构体

	uint32_t last_time; // 上次执行时间

	task_state_t state; // 任务状态

	int16_t next; // 指向下一个任务的索引
} task_t;


int16_t Task_Create(task_t * task);
void Task_Delete(task_t * task);
void Task_Suspend(task_t * task);
void Task_Resume(task_t * task);
void Task_Scheduler(void);

#endif /* _TASK_H_ */
