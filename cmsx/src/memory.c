//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "memory.h"

extern u16 g_HeapStartAddress;
u16 g_StackAddress;

//-----------------------------------------------------------------------------
/// Get the current address of the stack top (lower address)
u16 Mem_GetStackAddress()
{
	__asm
		ld		(_g_StackAddress), sp
		ld		hl, (_g_StackAddress)
	__endasm;
}

//-----------------------------------------------------------------------------
/// Get the current address of the heap top (higher addresse)
u16 Mem_GetHeapAddress()
{
	return g_HeapStartAddress;
}

//-----------------------------------------------------------------------------
/// Get the amount of free space in the heap
u16 Mem_GetHeapSize()
{ 
	return Mem_GetStackAddress() - Mem_GetHeapAddress();
}

//-----------------------------------------------------------------------------
/// Allocate a part of the heap
void* Mem_HeapAlloc(u16 size) __FASTCALL
{ 
	u16 addr = g_HeapStartAddress;
	g_HeapStartAddress += size;
	return (void*)addr;
}

//-----------------------------------------------------------------------------
/// Free the last allocated area of the heap
void Mem_HeapFree(u16 size) __FASTCALL
{
	g_HeapStartAddress -= size;	
}

//-----------------------------------------------------------------------------
/// Copy a memory block from a source address to an other
void Mem_Copy(const void* src, void* dest, u16 size)
{
	src, dest, size;
	__asm
		// Get parameters
		push	ix
		ld		ix, #0
		add		ix, sp
		ld		l, 4(ix)
		ld		h, 5(ix)	
		ld		e, 6(ix)
		ld		d, 7(ix)
		ld		c, 8(ix)
		ld		b, 9(ix)
		pop		ix
		// Skip if size == 0
		ld		a, b
		or		a, c
		ret		z
		// Do copy
		ldir
	_copy_end:

	__endasm;
}

//-----------------------------------------------------------------------------
/// Fill a memory block with a given value
void Mem_Set(void* dest, u8 val, u16 size)
{
	val, dest, size;
	__asm
		// Get parameters
		push	ix
		ld		ix, #0
		add		ix, sp
		ld		l, 4(ix)
		ld		h, 5(ix)
		ld		e, 6(ix)
		ld		c, 7(ix)
		ld		b, 8(ix)
		pop		ix
		// Skip if size == 0
		ld		a, b
		or		c
		ret		z
		// Set first parameter
		ld		(hl), e
		// Set DE to propagate change
		ld		e, l
		ld		d, h
		inc		de
		dec		bc
		// Skip if size == 0
		ld		a, b
		or		c
		ret		z
		// Do fill
		ldir
	__endasm;
}