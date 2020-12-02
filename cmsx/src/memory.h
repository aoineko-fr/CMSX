//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

#include "core.h"

// Get the current address of the stack top (lower address)
u16 Mem_GetStackAddress();

// Get the current address of the heap top (higher addresse)
u16 Mem_GetHeapAddress();

// Get the amount of free space in the heap
u16 Mem_GetHeapSize();

// Alloc a part of the heap
void* Mem_HeapAlloc(u16 size) __FASTCALL;

// Copy a memory block from a source address to an other
void Mem_Copy(void* src, void* dest, u16 size);