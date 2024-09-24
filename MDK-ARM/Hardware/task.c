#include "task.h"

volatile uint32_t time_counter = 0; // 时间计数器

#define NULL_TASK    (MAX_TASKS+1) // 空任务标识

task_t headTask =
{
	.next = NULL_TASK, // 初始化头任务的下一个任务为NULL_TASK
};
task_t * taskQueue[MAX_TASKS + 2] = {&headTask, NULL}; // 任务队列，包含头任务和空指针

volatile int16_t taskCount = 0; // 当前任务数量
volatile int16_t currentTask = 0; // 当前任务索引

/**
 * @brief 查找空闲任务槽
 * @return 空闲任务槽的索引
 */
static int16_t Task_Find_Empty(void)
{
	for(int16_t i = 1; i < NULL_TASK; i++)
	{
		if(taskQueue[i] == NULL)
		{
			return i;
		}
	}

	return NULL_TASK;
}

/**
 * @brief 设置任务状态
 * @param task 任务结构体
 * @param state 任务状态
 */
static void Task_Set_State(task_t * task, task_state_t state)
{
	if(task == NULL)
	{
		task = taskQueue[currentTask];
	}

	task->state = state; // 设置状态
}

/**
 * @brief 创建一个任务
 * @param task 任务结构体
 * @return 当前任务数量
 */
int16_t Task_Create(task_t * task)
{
	if(task == NULL)
	{
		return -1;
	}

	currentTask = 0;

	while(taskQueue[currentTask]->next != NULL_TASK)
	{
		currentTask = taskQueue[currentTask]->next;
	}

	if(taskCount < MAX_TASKS)
	{
		taskQueue[currentTask]->next = Task_Find_Empty();
		task->next = NULL_TASK;
		Task_Set_State(task, TASK_RUNNING);
		taskQueue[taskQueue[currentTask]->next] = task;

		taskCount++;
		return taskCount;
	}
	else
	{
		return -1;
	}
}

/**
 * @brief 标记一个任务为删除
 * @param task 任务结构体
 */
void Task_Delete(task_t * task)
{
	Task_Set_State(task, TASK_DELETED);
}

/**
 * @brief 挂起一个任务
 * @param task 任务结构体
 */
void Task_Suspend(task_t * task)
{
	Task_Set_State(task, TASK_SUSPENDED);
}

/**
 * @brief 恢复一个任务
 * @param task 任务结构体
 */
void Task_Resume(task_t * task)
{
	Task_Set_State(task, TASK_RUNNING);
}

/**
 * @brief 删除标记为删除的任务
 */
void Task_Scheduler_Delete()
{
	currentTask = 0;

	while(taskQueue[currentTask]->next != NULL_TASK)
	{
		int16_t lastTask = 0;
		lastTask = currentTask;
		currentTask = taskQueue[currentTask]->next;

		if(taskQueue[currentTask]->state == TASK_DELETED)
		{
			taskQueue[lastTask]->next = taskQueue[currentTask]->next;
			taskQueue[currentTask] = NULL;
			taskCount--;
			currentTask = lastTask;
		}
	}
}

/**
 * @brief 执行任务
 */
void Task_Execute(void)
{
	currentTask = 0;

	while(taskQueue[currentTask]->next != NULL_TASK)
	{
		currentTask = taskQueue[currentTask]->next;

		if(taskQueue[currentTask]->state == TASK_RUNNING && (time_counter - taskQueue[currentTask]->last_time) >= taskQueue[currentTask]->init.period)
		{
			taskQueue[currentTask]->last_time = time_counter;
			taskQueue[currentTask]->init.task_func();
		}
	}
}

/**
 * @brief 任务调度器，运行此函数后，代码会进入任务调度循环，后续代码不会执行
 */
void Task_Scheduler(void)
{
	static uint32_t last_time;
	last_time = time_counter;

	while(1)
	{
		if(time_counter != last_time)
		{
			last_time = time_counter;
			Task_Execute();
			Task_Scheduler_Delete();
		}
	}
}
