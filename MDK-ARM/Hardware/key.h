#ifndef _KEY_H_
#define _KEY_H_

#include "stdbool.h"
#include "stdint.h"



enum KeyStatus
{
	HOVER, // 按键悬停状态
	DEBOUNCE,//去抖动
	PRESSED // 按键按下状态
};

enum KeyFlag
{
	NONE = 0,
	SINGLE_CLICK = 0x0001, // 单击
	DOUBLE_CLICK = 0x0002, // 双击
	LONG_PRESSRD = 0x0004 // 长按
};

typedef bool (*IsKeyPressedFunc)(void);

struct test;

typedef struct
{
	IsKeyPressedFunc is_key_pressed; // 检查按键是否按下的函数指针
	enum KeyFlag flag; // 按键可触发的标志
} Key_init_t;

typedef struct Key_t
{
	Key_init_t init;

	bool pressed;
	bool lock;
	enum KeyStatus status; // 按键状态
	enum KeyFlag triggered; // 已触发的按键标志
	uint32_t pressed_time;//按下的时间
	uint32_t interval_time;//两次点击间隔时间
} Key_t;

void Key_init(Key_t * key);
void Key_manage(void);

bool Key_is_flag_set(Key_t * key, enum KeyFlag flag);


#endif // !_KEY_H_
