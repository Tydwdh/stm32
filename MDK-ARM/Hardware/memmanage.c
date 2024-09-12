#include "memmanage.h"

// 每字节的位数
#define heapBITS_PER_BYTE		( ( size_t ) 8 )

// 定义堆的大小
static uint8_t heap[ TOTAL_HEAP_SIZE ];

// 定义块链接结构体
typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK * nextFreeBlock; // 指向下一个空闲块的指针
	size_t blockSize; // 块的大小
} BlockLink_t;

// 定义起始块和结束块
static BlockLink_t start, * end = NULL;

// 计算块结构体的大小，并进行字节对齐
static const size_t heapStructSize	= (sizeof(BlockLink_t) + ((size_t)(BYTE_ALIGNMENT - 1))) & ~((size_t) BYTE_ALIGNMENT_MASK);

// 定义最小块大小
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( heapStructSize << 1 ) )

// 剩余空闲字节数
static size_t freeBytesRemaining = 0U;
// 记录历史最小空闲字节数
static size_t minimumEverFreeBytesRemaining = 0U;

// 块分配标志位
static size_t blockAllocatedBit = 0;

// 堆初始化函数
static void Heap_init(void);

static void Heap_init(void)
{
	BlockLink_t * firstFreeBlock;
	uint8_t * alignedHeap;
	size_t address;
	size_t totalHeapSize = TOTAL_HEAP_SIZE;

	// 获取堆的起始地址
	address = (size_t)(heap);

	// 如果地址未对齐，则进行对齐处理
	if((address & BYTE_ALIGNMENT_MASK) != 0)
	{
		address += (BYTE_ALIGNMENT - 1);
		address &= ~((size_t) BYTE_ALIGNMENT_MASK);
		totalHeapSize -= address - (size_t) heap;
	}

	alignedHeap = (uint8_t *) address;

	// 初始化起始块
	start.nextFreeBlock = (BlockLink_t *) alignedHeap;
	start.blockSize = (size_t) 0;

	// 计算结束块的地址，并进行对齐处理
	address = ((size_t) alignedHeap) + totalHeapSize;
	address -= heapStructSize;
	address &= ~((size_t) BYTE_ALIGNMENT_MASK);
	end = (void *) address;
	end->blockSize = 0;
	end->nextFreeBlock = NULL;

	// 初始化第一个空闲块
	firstFreeBlock = (void *) alignedHeap;
	firstFreeBlock->blockSize = address - (size_t) firstFreeBlock;
	firstFreeBlock->nextFreeBlock = end;

	// 记录初始空闲字节数
	minimumEverFreeBytesRemaining = firstFreeBlock->blockSize;
	freeBytesRemaining = firstFreeBlock->blockSize;

	// 设置块分配标志位
	blockAllocatedBit = ((size_t) 1) << ((sizeof(size_t) * heapBITS_PER_BYTE) - 1);
}

// 将块插入空闲链表
static void Insert_Block_IntoFreeList(BlockLink_t * blockToInsert)
{
	BlockLink_t * iterator;
	uint8_t * puc;

	// 找到合适的位置插入块
	for(iterator = &start; iterator->nextFreeBlock < blockToInsert; iterator = iterator->nextFreeBlock)
	{
	}

	puc = (uint8_t *) iterator;

	// 如果块可以与前一个块合并，则合并
	if((puc + iterator->blockSize) == (uint8_t *) blockToInsert)
	{
		iterator->blockSize += blockToInsert->blockSize;
		blockToInsert = iterator;
	}

	puc = (uint8_t *) blockToInsert;

	// 如果块可以与后一个块合并，则合并
	if((puc + blockToInsert->blockSize) == (uint8_t *) iterator->nextFreeBlock)
	{
		if(iterator->nextFreeBlock != end)
		{
			blockToInsert->blockSize += iterator->nextFreeBlock->blockSize;
			blockToInsert->nextFreeBlock = iterator->nextFreeBlock->nextFreeBlock;
		}
		else
		{
			blockToInsert->nextFreeBlock = end;
		}
	}
	else
	{
		blockToInsert->nextFreeBlock = iterator->nextFreeBlock;
	}

	// 更新链表
	if(iterator != blockToInsert)
	{
		iterator->nextFreeBlock = blockToInsert;
	}
}

// 分配内存
void * MEM_malloc(size_t wantedSize)
{
	BlockLink_t * block, * previousBlock, * newBlockLink;
	void * Return = NULL;

	// 如果堆未初始化，则初始化堆
	if(end == NULL)
	{
		Heap_init();
	}

	// 检查请求的大小是否有效
	if((wantedSize & blockAllocatedBit) == 0)
	{
		if(wantedSize > 0)
		{
			wantedSize += heapStructSize;

			// 进行字节对齐
			if((wantedSize & BYTE_ALIGNMENT_MASK) != 0x00)
			{
				wantedSize += (BYTE_ALIGNMENT - (wantedSize & BYTE_ALIGNMENT_MASK));
			}

			// 检查是否有足够的空闲内存
			if((wantedSize > 0) && (wantedSize <= freeBytesRemaining))
			{
				previousBlock = &start;
				block = start.nextFreeBlock;

				// 找到合适的空闲块
				while((block->blockSize < wantedSize) && (block->nextFreeBlock != NULL))
				{
					previousBlock = block;
					block = block->nextFreeBlock;
				}

				// 如果找到合适的块，则分配内存
				if(block != end)
				{
					Return = (void *)(((uint8_t *) previousBlock->nextFreeBlock) + heapStructSize);

					previousBlock->nextFreeBlock = block->nextFreeBlock;

					// 如果剩余的块大小大于最小块大小，则拆分块
					if((block->blockSize - wantedSize) > heapMINIMUM_BLOCK_SIZE)
					{
						newBlockLink = (void *)(((uint8_t *) block) + wantedSize);
						newBlockLink->blockSize = block->blockSize - wantedSize;
						block->blockSize = wantedSize;
						Insert_Block_IntoFreeList(newBlockLink);
					}

					// 更新空闲字节数
					freeBytesRemaining -= block->blockSize;

					if(freeBytesRemaining < minimumEverFreeBytesRemaining)
					{
						minimumEverFreeBytesRemaining = freeBytesRemaining;
					}

					// 设置块分配标志位
					block->blockSize |= blockAllocatedBit;
					block->nextFreeBlock = NULL;
				}
			}
		}
	}

	return Return;
}

// 释放内存
void MEM_free(void * ptr)
{
	uint8_t * puc = (uint8_t *) ptr;
	BlockLink_t * link;

	if(ptr != NULL)
	{
		// 获取块的起始地址
		puc -= heapStructSize;
		link = (void *) puc;

		// 检查块是否已分配
		if((link->blockSize & blockAllocatedBit) != 0)
		{
			if(link->nextFreeBlock == NULL)
			{
				// 清除块分配标志位
				link->blockSize &= ~blockAllocatedBit;
				// 更新空闲字节数
				freeBytesRemaining += link->blockSize;
				// 将块插入空闲链表
				Insert_Block_IntoFreeList(((BlockLink_t *) link));
			}
		}
	}
}
