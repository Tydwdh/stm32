/**
 * @file struct ring_buffer.h
 * @author tyd
 * @brief 环形缓冲区, 实现了环形缓冲区的基本操作,参考linux的kfifo实现
 * @version 0.1
 * @date 2024-08-05
 */


#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "memmanage.h"


#ifndef MALLOC
	#define MALLOC(size) MEM_malloc(size)
#endif // !


#ifndef MIN
	#define MIN(x,y) ((x) < (y) ? x : y)
#endif

#ifndef MAX
	#define MAX(x,y) ((x) > (y) ? x : y)
#endif

#define IS_POWER_OF_2(n) (n != 0 && ((n & (n - 1)) == 0))

typedef struct
{
	uint16_t esize; /*表示每个元素的大小（以字节为单位）*/
	uint16_t in; /* 数据从in位置开始写入 (in % size) */
	uint16_t mask; /* 用于计算索引的掩码值，是缓冲区大小减一 */
	uint16_t out; /* 数据从out位置开始读取. (out % size) */
	void * buffer; /* 存储缓冲区的指针 */
} ring_buffer;



int16_t RingBuffer_init(ring_buffer * rb, uint8_t * buffer, uint16_t size, size_t esize);
int16_t RingBuffer_alloc(ring_buffer * rb, uint16_t size, size_t esize);

uint16_t RingBuffer_in(ring_buffer * rb, const void * data, uint16_t len);
uint16_t RingBuffer_out(ring_buffer * rb, void * data, uint16_t len);
uint16_t RingBuffer_out_peek(ring_buffer * rb, void * data, uint16_t len);

uint16_t RingBuffer_used(ring_buffer * rb);
uint16_t RingBuffer_unused(ring_buffer * rb);

void RingBuffer_free(ring_buffer * rb);

#endif


