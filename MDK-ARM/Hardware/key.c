#include "key.h"


Key_t * keys[10];
uint16_t key_number = 0;

#define LONG_PRESSRD_TIME 80
#define INTERVAL_TIME 20

/**
 * @brief 按键初始化
 * @param key 按键控制结构体
 */
void Key_init(Key_t * key)
{
	if(key == NULL)
	{
		return;
	}

	key->lock = true;
	keys[key_number++] = key;
}

static void handle_hover_state(Key_t * key);
static void handle_debounce_state(Key_t * key);
static void handle_pressed_state(Key_t * key);
static void interval_time_increase(Key_t * key);

/**
 * @brief 按键逻辑管理函数,请间隔固定时间调用此函数
 */
void Key_manage()
{
	for(int i = 0; i < key_number; i++)
	{
		keys[i]->pressed = keys[i]->init.is_key_pressed();

		switch(keys[i]->status)
		{
			case HOVER:
				handle_hover_state(keys[i]);
				break;

			case DEBOUNCE:
				handle_debounce_state(keys[i]);
				break;

			case PRESSED:
				handle_pressed_state(keys[i]);
				break;

			default:
				break;
		}
	}
}

void handle_hover_state(Key_t * key)
{
	if(key->pressed)
	{
		key->status = DEBOUNCE;
	}

	interval_time_increase(key);


	if(!key->lock && key->interval_time > INTERVAL_TIME)
	{
		key->triggered |= SINGLE_CLICK;
		key->lock = true;
	}
}

void handle_debounce_state(Key_t * key)
{
	if(key->pressed)
	{
		key->status = PRESSED;
		key->lock = false;
	}
	else
	{
		key->status = HOVER;
	}

	interval_time_increase(key);
}

void handle_key_release(Key_t * key)
{
	key->status = HOVER;
	key->pressed_time = 0;

	if(key->lock)
	{
		return;
	}

	if((key->init.flag & DOUBLE_CLICK) && key->interval_time <= INTERVAL_TIME)
	{
		key->triggered |= DOUBLE_CLICK;
		key->interval_time = INTERVAL_TIME + 1;
		key->lock = true;
	}
	else if((key->init.flag & ~LONG_PRESSRD) == SINGLE_CLICK)
	{
		key->triggered |= SINGLE_CLICK;
		key->lock = true;
	}
	else
	{
		key->interval_time = 0;
	}
}


void handle_key_preased(Key_t * key)
{
	key->pressed_time++;

	if(key->pressed_time >= LONG_PRESSRD_TIME)
	{
		key->triggered |= LONG_PRESSRD;
		key->interval_time = INTERVAL_TIME + 1;
		key->lock = true;
	}
}

void handle_pressed_state(Key_t * key)
{
	if(!key->pressed)
	{
		handle_key_release(key);
	}
	else if(!key->lock && (key->init.flag & LONG_PRESSRD))
	{
		handle_key_preased(key);
	}
}


bool Key_is_flag_set(Key_t * key, enum KeyFlag flag)
{
	if(key->triggered & flag)
	{
		key->triggered &= ~flag;
		return true;
	}

	return false;
}

static void interval_time_increase(Key_t * key)
{
	if(key->interval_time <= INTERVAL_TIME)
	{
		key->interval_time++;
	}
}
