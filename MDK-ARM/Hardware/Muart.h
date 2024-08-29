#ifndef _MUART_H_
#define _MUART_H_


#include "stm32f1xx_hal.h"
#include "RingBuffer.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"


#define MAX_RX_LEN 256

// 函数声明
extern uint8_t rx_buffer[MAX_RX_LEN];


typedef struct
{
    UART_HandleTypeDef *huart;
    ring_buffer *rb_rx;
    ring_buffer *rb_tx;
} muart_handle;



void MUART_Data_Process(muart_handle * muart);
void MUART_Data_Transimit(muart_handle * muart);
void MUART_Printf(muart_handle * muart, const char *fmt, ...);

#endif
