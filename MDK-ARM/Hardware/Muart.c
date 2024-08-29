#include "Muart.h"
#include "stdarg.h"
#include "main.h"


uint8_t rx_buffer[MAX_RX_LEN];
uint8_t data[MAX_RX_LEN];



__weak void MUART_Data_Process(muart_handle * muart)
{
    //简单收到什么返回什么
    while(RingBuffer_used(muart->rb_rx) != 0)
    {
        uint16_t len = RingBuffer_used(muart->rb_rx);
        uint8_t data[MAX_RX_LEN];
        RingBuffer_out(muart->rb_rx, data, len);
        RingBuffer_in(muart->rb_tx, data, len);
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
        RingBuffer_out(muart->rb_tx, data, len);
        HAL_UART_Transmit_DMA(muart->huart, data, len);
    }
}




char buf[256];
/**
 * @brief
 * @param fmt
 * @param
 */
void MUART_Printf(muart_handle * muart, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    RingBuffer_in(muart->rb_tx, (uint8_t*)buf, strlen(buf));
}


// UART接收空闲中断回调函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size)
{
    if(huart->Instance == USART1)
    {
        RingBuffer_in(muart1.rb_rx, rx_buffer, Size);
        HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, MAX_RX_LEN * 2);
    }

    // else if(huart->Instance == USART2)
    // {
    //     RingBuffer_in(muart2.rb_rx, rx_buffer, Size);
    //     HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, MAX_RX_LEN * 2);
    // }
}




