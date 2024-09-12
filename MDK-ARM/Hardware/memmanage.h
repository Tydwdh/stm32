#ifndef __MEMMANAGE_H__
#define __MEMMANAGE_H__


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define TOTAL_HEAP_SIZE 4096

#define BYTE_ALIGNMENT 4

#if BYTE_ALIGNMENT == 32
	#define BYTE_ALIGNMENT_MASK ( 0x001f )
#endif

#if BYTE_ALIGNMENT == 16
	#define BYTE_ALIGNMENT_MASK ( 0x000f )
#endif
 
#if BYTE_ALIGNMENT == 8
	#define BYTE_ALIGNMENT_MASK ( 0x0007 )
#endif

#if BYTE_ALIGNMENT == 4
	#define BYTE_ALIGNMENT_MASK	( 0x0003 )
#endif

#if BYTE_ALIGNMENT == 2
	#define BYTE_ALIGNMENT_MASK	( 0x0001 )
#endif

#if BYTE_ALIGNMENT == 1
	#define BYTE_ALIGNMENT_MASK	( 0x0000 )
#endif

#ifndef BYTE_ALIGNMENT_MASK
	#error "Invalid BYTE_ALIGNMENT definition"
#endif



void* MEM_malloc(size_t wantedSize);

void MEM_free(void* ptr);



#endif // !__MEMMANAGE_H__
