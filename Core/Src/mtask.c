#include "mtask.h"





static void Sys_init(void);
task_t sysTask =
{
	.task_func = Sys_init,
	.period = 0,
};

static void Uart_data_handler(void);


task_t uartTask =
{
	.task_func = Uart_data_handler,
	.period = 10,
};



void MTask_Init(void)
{
	Task_Create(&sysTask);
	Task_Create(&uartTask);
}



static void Sys_init(void)
{
	MUART_Init();

	Task_Delete(&sysTask);
}

static void Uart_data_handler(void)
{
	MUART_Data_Process(&muart1);
	MUART_Data_Transimit(&muart1);
}


