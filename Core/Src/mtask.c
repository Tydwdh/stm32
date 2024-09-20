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

static void Key_handler(void);
static bool Key1_check(void);
Key_t key1;
task_t keyTask =
{
	.task_func = Key_handler,
	.period = 10,
};



void MTask_Init(void)
{
	Task_Create(&sysTask);
	Task_Create(&uartTask);
	Task_Create(&keyTask);
}



static void Sys_init(void)
{
	MUART_Init();


	key1.init.flag = (enum KeyFlag)(SINGLE_CLICK | DOUBLE_CLICK | LONG_PRESSRD);
	key1.init.is_key_pressed = Key1_check;
	Key_init(&key1);

	Task_Delete(&sysTask);
}

static void Uart_data_handler(void)
{
	MUART_Data_Process(&muart1);
	MUART_Data_Transimit(&muart1);
}

static void Key_handler(void)
{
	Key_manage();

	if(Key_is_flag_set(&key1, SINGLE_CLICK))
	{
		MUART_Printf(&muart1, "单击");
	}

	if(Key_is_flag_set(&key1, DOUBLE_CLICK))
	{
		MUART_Printf(&muart1, "双击");
	}

	if(Key_is_flag_set(&key1, LONG_PRESSRD))
	{
		MUART_Printf(&muart1, "长按");
	}
}

static bool Key1_check(void)
{
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
}
