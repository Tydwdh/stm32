#include "RingBuffer.h"



/**
 * @brief 向上取2的整数次幂
 * @param x  输入值
 * @return  x的上取2的整数次幂
 */
static uint16_t ceil_power_of_two(uint16_t x)
{
	if(x <= 1)
	{
		return 2;
	}

	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	return x + 1;
}

/**
 * @brief  向下取2的整数次幂
 * @param x  输入值
 * @return  x的下取2的整数次幂
 */
static uint16_t floor_power_of_two(uint16_t x)
{
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	return x - (x >> 1);
}



/**
 * @brief 根据给定的buffer和size，初始化RingBuffer
 * @param buffer 给定的buffer
 * @param size 给定的buffer的内存大小
 * @note 这里的buffer是指一段连续的内存，并不是一个指针数组
 * @warning size必须是2的整数次幂
 * @return 初始化成功返回实际buffer的大小，失败返回-1
 */
int16_t RingBuffer_init(ring_buffer * rb, uint8_t * buffer, uint16_t size, size_t esize)
{
	size /= esize;

	if(!IS_POWER_OF_2(size))
	{
		size = floor_power_of_two(size);
	}

	rb->in = 0;
	rb->out = 0;
	rb->esize = esize;
	rb->buffer = buffer;

	if(size < 2)
	{
		return -1;
	}

	rb->mask = size - 1;

	return size;
}



/**
 * @brief 根据指定大小动态分配RingBuffer内存
 * @param size  RingBuffer可存储数据的个数
 * @param esize  RingBuffer中每个元素的大小
 * @note 只适用于有操作系统的环境,请使用对应的malloc函数
 * @return  成功返回实际buffer的大小，失败返回-1
 */
int16_t RingBuffer_alloc(ring_buffer * rb, uint16_t size, size_t esize)
{
	if(!IS_POWER_OF_2(size))
	{
		size = ceil_power_of_two(size);
	}


	rb->in = 0;
	rb->out = 0;
	rb->esize = esize;

	if(size < 2)
	{
		rb->buffer = NULL;
		rb->mask = 0;
		return -1;
	}

	rb->buffer = MALLOC(size * esize);

	if(!rb->buffer)
	{
		rb->mask = 0;
		return -1;
	}

	rb->mask = size - 1;

	return size;
}

/**
 * @brief 释放由RingBuffer_Alloc分配的RingBuffer
 * @param rb  RingBuffer指针
 */
void RingBuffer_free(ring_buffer * rb)
{
	free(rb->buffer);
	rb->in = 0;
	rb->out = 0;
	rb->esize = 0;
	rb->buffer = NULL;
	rb->mask = 0;
}

static void RingBuffer_copy_in(ring_buffer * rb, const void * src, uint16_t len, uint16_t off)
{
	uint16_t size = rb->mask + 1;
	uint16_t esize = rb->esize;
	uint16_t l;

	off &= rb->mask;

	if(esize != 1)
	{
		off *= esize;
		size *= esize;
		len *= esize;
	}

	l = MIN(len, size - off);

	memcpy((uint8_t *)rb->buffer + off, src, l);
	memcpy(rb->buffer, (uint8_t *)src + l, len - l);
}

static void RingBuffer_copy_out(ring_buffer * rb, void * dst, uint16_t len, uint16_t off)
{
	uint16_t size = rb->mask + 1;
	uint16_t esize = rb->esize;
	uint16_t l;

	off &= rb->mask;

	if(esize != 1)
	{
		off *= esize;
		size *= esize;
		len *= esize;
	}

	l = MIN(len, size - off);

	memcpy(dst, (uint8_t *)rb->buffer + off, l);
	memcpy((uint8_t *)dst + l, rb->buffer, len - l);
}

/**
 * @brief 向RingBuffer中写入数据
 * @param rb RingBuffer指针
 * @param data 要写入的数据
 * @param len  要写入的数据长度
 * @return 实际写入的长度
 */
uint16_t RingBuffer_in(ring_buffer * rb, const void * data, uint16_t len)
{
	uint16_t l = RingBuffer_unused(rb);

	if(l < len) //空闲空间不够,只存储部分数据
	{
		len = l;
	}

	RingBuffer_copy_in(rb, data, len, rb->in);
	rb->in += len;

	return len;
}

/**
 * @brief  从RingBuffer中读取数据
 * @param rb  RingBuffer指针
 * @param data  读取到的数据
 * @param len  要读取的数据长度
 * @return  实际读取的长度
 */
uint16_t RingBuffer_out(ring_buffer * rb, void * data, uint16_t len)
{
	len = RingBuffer_out_peek(rb, data, len);
	rb->out += len;
	return len;
}

/**
 * @brief
 * @param rb
 * @param data
 * @param len
 * @return
 */
uint16_t RingBuffer_out_peek(ring_buffer * rb, void * data, uint16_t len)
{
	uint16_t l;

	l = RingBuffer_used(rb);

	if(len > l)
	{
		len = l;
	}

	RingBuffer_copy_out(rb, data, len, rb->out);
	return len;
}

/**
 * @brief  获取RingBuffer中数据长度
 * @param rb  RingBuffer指针
 * @return  RingBuffer中数据长度
 */
uint16_t RingBuffer_used(ring_buffer * rb)
{
	return rb->in - rb->out;
}

/**
 * @brief  获取RingBuffer剩余空间长度
 * @note  RingBuffer剩余空间长度 = RingBuffer总长度 - RingBuffer已用长度
 * @param rb  RingBuffer指针
 * @return  RingBuffer剩余空间长度
 */
uint16_t RingBuffer_unused(ring_buffer * rb)
{
	return rb->mask + 1 - rb->in + rb->out;
}


