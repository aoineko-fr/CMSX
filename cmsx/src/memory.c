//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "memory.h"

extern u16 g_HeapStartAddress;
u16 g_StackAddress;

//-----------------------------------------------------------------------------
// Get the current address of the stack top (lower address)
u16 Mem_GetStackAddress()
{
	__asm
		ld		(_g_StackAddress), sp
		ld		hl, (_g_StackAddress)
	__endasm;
}

//-----------------------------------------------------------------------------
// Get the current address of the heap top (higher addresse)
u16 Mem_GetHeapAddress()
{
	return g_HeapStartAddress;
}

//-----------------------------------------------------------------------------
// Get the amount of free space in the heap
u16 Mem_GetHeapSize()
{ 
	return Mem_GetStackAddress() - Mem_GetHeapAddress();
}

//-----------------------------------------------------------------------------
// Allocate a part of the heap
void* Mem_HeapAlloc(u16 size) __FASTCALL
{ 
	u16 addr = g_HeapStartAddress;
	g_HeapStartAddress += size;
	return (void*)addr;
}

//-----------------------------------------------------------------------------
// Copy a memory block from a source address to an other
void Mem_Copy(void* src, void* dest, u16 size)
{
	src, dest, size;
	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		ld		c, 8(ix)
		ld		b, 9(ix)
		ld		a, b
		or		a, c
		jp		z, _copy_end

		ld		l, 4(ix)
		ld		h, 5(ix)	
		ld		e, 6(ix)
		ld		d, 7(ix)
		ldir
	_copy_end:

		pop		ix
	__endasm;
}
