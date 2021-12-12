//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________

#include "system.h"

//-----------------------------------------------------------------------------
/// Get the slot ID of a given page
u8 Sys_GetSlot(u8 page)
{
	u8 slot = (g_PortPrimarySlot >> (page * 2)) & 0x03;
	if(g_EXPTBL[slot] & 0x80)
	{
		slot |= SLOT_EXP;
		slot |= (((~g_SLTSL) >> (page * 2)) & 0x03) << 2; // @todo Fix it! Page 3 must to be set to slot id to be able to retreive subslot
	}
	return slot;
}

//-----------------------------------------------------------------------------
/// Set the slot ID of a given page
void Sys_SetSlot(u8 page, u8 slot)
{
/*

; Select the slot 2-1 for page 1 (4000h-07FFFh) using BIOS routine
ENASLT	equ	0024h

	ld	h,040h
	ld	a,086h	;Slot ID
	call	ENASLT

; Select the slot 2-1 for page 1 (4000h-07FFFh) using direct accesses (not recommended by the standard)
	in	a,(0A8h)	;Read primary slots register
	ld	b,a		;Store current configuration
	and	00110011b	;Resets the bits of pages 4000h-07FFFh and C000h-0FFFFh
	or	10001000b	;Sets the bits of pages 4000h-07FFFh and C000h-0FFFF to 2
	di
	out	(0A8h),a	;Select primary slot 2 for the pages 4000h-07FFFh and C000h-0FFFFh

	ld	a,(0FFFFh)	;Read secondary slots register of selected primary slot
	cpl			;Reverses the bits
	and	11110011b	;Resets the bits of page 4000h-07FFFh 
	or	00000100b	;Sets the bits of page 4000h-07FFFh to 1
	ld	(0FFFFh),a	;Select secondary slot 2-1

	ld	a,b		;Restore initial configuration of primary slots to A
	and	11000000b	;Keep only the bits of page C000h-0FFFFh
	ld	b,a		;Store the bits of page C000h-0FFFFh to B
	in	a,(0A8h)	;Read primary slots register
	and	00111111b	;Resets the bits of page C000h-0FFFFh
	or	b		;Sets the bits of page C000h-0FFFF to initial value
	out	(0A8h),a	;Select initial primary slot for the page C000h-0FFFFh
	ei
*/
}
