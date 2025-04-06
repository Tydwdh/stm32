#include "mtask.h"



static void Sys_init(void);
task_t sysTask =
{
	.init.task_func = Sys_init,
	.init.period = 0,
};


static void Uart_data_handler(void);
task_t uartTask =
{
	.init.task_func = Uart_data_handler,
	.init.period = 10,
};

static void Key_handler(void);
static bool Key1_check(void);
Key_t key1;//GPIOB, GPIO_PIN_3


task_t keyTask =
{
	.init.task_func = Key_handler,
	.init.period = 10,
};



void MTask_Init(void)
{
	Task_Create(&sysTask);//系统初始化
	Task_Create(&uartTask);//串口任务
	Task_Create(&keyTask);//按键任务
}



static void Sys_init(void)
{
	MUART_Init();//串口初始化 PA9 PA10 115200


	key1.init.flag = (enum KeyFlag)(SINGLE_CLICK | DOUBLE_CLICK | LONG_PRESSRD);
	key1.init.is_key_pressed = Key1_check;
	Key_init(&key1);//按键初始化

	Task_Delete(NULL);//该任务只执行一次,执行后删除
}

static void Uart_data_handler(void)
{
	MUART_Data_Process(&muart1);//串口接收数据处理,可自行编写
	MUART_Data_Transimit(&muart1);//将串口缓冲区的数据发送
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
