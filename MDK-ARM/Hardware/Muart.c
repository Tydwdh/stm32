#include "Muart.h"
#include "stdarg.h"
#include "main.h"
#include "usart.h"

uint8_t rx_buffer[MAX_RX_LEN];


muart_handle muart1;
static uint8_t buffer[MAX_RX_LEN];
static uint8_t txbuffer[MAX_RX_LEN];
//使用DMA空闲中断接收数据建议将DMA的半完成中断中的HAL_UARTEx_RxEventCallback注释掉
void MUART_Init(void)
{
	muart1.rb_rx = (ring_buffer *)MEM_malloc(sizeof(ring_buffer));
	muart1.rb_tx = (ring_buffer *)MEM_malloc(sizeof(ring_buffer));
	RingBuffer_alloc(muart1.rb_rx, 1024, 1);
	RingBuffer_alloc(muart1.rb_tx, 1024, 1);

	muart1.huart = &huart1;

	HAL_UARTEx_ReceiveToIdle_DMA(muart1.huart, rx_buffer, MAX_RX_LEN);
}


__weak void MUART_Data_Process(muart_handle * muart)
{
	//简单收到什么返回什么
	while(RingBuffer_used(muart->rb_rx) != 0)
	{
		uint16_t len = RingBuffer_used(muart->rb_rx);
		RingBuffer_out(muart->rb_rx, buffer, len);
		RingBuffer_in(muart->rb_tx, buffer, len);
	}
}

/**
 * @brief 只要发送缓冲区存在数据并且串口发送空闲就会发送数据,请放在while循环中使用,或者任务中
 * @param huart 串口总控结构体
 */
void MUART_Data_Transimit(muart_handle * muart)
{
	while(RingBuffer_used(muart->rb_tx) != 0 && muart->huart->gState == HAL_UART_STATE_READY)
	{
		uint16_t len = RingBuffer_used(muart->rb_tx);
		RingBuffer_out(muart->rb_tx, txbuffer, len);
		HAL_UART_Transmit_DMA(muart->huart, txbuffer, len);
	}
}



/**
 * @brief
 * @param fmt
 * @param
 */
void MUART_Printf(muart_handle * muart, const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsnprintf((char *)buffer, sizeof(buffer), fmt, args);
	va_end(args);
	RingBuffer_in(muart->rb_tx, buffer, strlen((char *)buffer));
}


// UART接收空闲中断回调函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t Size)
{
	if(huart->Instance == muart1.huart->Instance)
	{
		RingBuffer_in(muart1.rb_rx, rx_buffer, Size);
		HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, MAX_RX_LEN);
	}
}


