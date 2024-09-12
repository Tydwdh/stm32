#include "task.h"



volatile uint32_t time_counter = 0;

#include "stdbool.h"
task_t * taskQueue[MAX_TASKS] = {NULL};
task_t * taskDeleted[MAX_TASKS] = {NULL};

volatile uint8_t taskdeleteCount = 0;
volatile uint8_t taskCount = 0;



/**
 * @brief 创建一个任务
 * @param task 任务结构体
 * @return 当前任务数量
 */
int16_t Task_Create(task_t * task)
{
	if(taskCount < MAX_TASKS)
	{
		taskQueue[taskCount] = task;
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
	taskDeleted[taskdeleteCount] = task;
	taskdeleteCount++;
}

/**
 * @brief 删除标记为删除的任务
 */
void Task_Scheduler_Delete()
{
	for(int16_t i = 0; i < taskdeleteCount; i++)
	{
		for(int16_t j = 0; j < taskCount; j++)
		{
			if(taskQueue[j] == taskDeleted[i])
			{
				taskCount--;

				for(int16_t k = j; k < taskCount; k++)
				{
					taskQueue[k] = taskQueue[k + 1];
				}

				taskQueue[taskCount] = NULL; // 清除最后一个任务指针
			}
		}
	}

	taskdeleteCount = 0; // 清空删除队列
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

			for(uint8_t i = 0; i < taskCount; i++)
			{
				if(time_counter - taskQueue[i]->last_time >= taskQueue[i]->period)
				{
					taskQueue[i]->last_time = time_counter;
					taskQueue[i]->task_func();
				}
			}

			Task_Scheduler_Delete();
		}
	}
}
