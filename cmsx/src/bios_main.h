//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// MAIN-ROM ROUTINES WRAPPER
//
// References:
// - MSX2 Technical Handbook
// - http://map.grauw.nl/resources/msxbios.php
// - https://www.msx.org/wiki/Main-ROM_BIOS
//-----------------------------------------------------------------------------
#pragma once

#include "bios.h"

//-----------------------------------------------------------------------------
// Helper functions

// Call a bios function
inline void Bios_MainCall(u16 addr);

//=============================================================================
// RST-and other routines
//=============================================================================

// CHKRAM (STARTUP, RESET, BOOT)	#0000	Tests RAM and sets RAM slot for the system
inline void Bios_Startup();

// SYNCHR	#0008	Tests whether the character of (HL) is the specified character

// RDSLT	#000C	Reads the value of an address in another slot
u8 Bios_InterSlotRead(u8 slot, u16 addr);
u8 Bios_MainROMRead(u16 addr) __FASTCALL;

// CHRGTR	#0010	Gets the next character (or token) of the Basic text

// WRSLT	#0014	Writes a value to an address in another slot
void Bios_InterSlotWrite(u8 slot, u16 addr, u8 value);

// OUTDO	#0018	Output to current output channel (printer, file, etc.)

// CALSLT	#001C	Executes inter-slot call
void Bios_InterSlotCall(u8 slot, u16 addr);
void Bios_MainROMCall(u16 addr) __FASTCALL;

// DCOMPR	#0020	Compares HL with DE
// ENASLT	#0024	Switches indicated slot at indicated page on perpetually
// GETYPR	#0028	Returns Type of DAC

// CALLF	#0030	Executes an interslot call
#define Bios_InterSlotCallF(_slot, _addr) \
	__asm                                 \
		rst		R_CALLF                   \
		db		_slot                     \
		dw		_addr                     \
	__endasm;

// KEYINT	#0038	Executes the timer interrupt process routine

//=============================================================================
// Initialization-routines
//=============================================================================

// INITIO	#003B	Initialises the device
// INIFNK	#003E	Initialises the contents of the function keys

//=============================================================================
// VDP routines
//=============================================================================
#if USE_BIOS_VDP

// DISSCR	#0041	Inhibits the screen display
inline void Bios_DisableScreen();

// ENASCR	#0044	Displays the screen
inline void Bios_EnableScreen();

// WRTVDP	#0047	Write data in the VDP-register
void Bios_WriteVDP(u8 reg, u8 value);

// RDVRM	#004A	Reads the content of VRAM
u8 Bios_ReadVRAM(u16 addr) __FASTCALL;

// WRTVRM	#004D	Writes data in VRAM
void Bios_WriteVRAM(u16 addr, u8 value);

// SETRD	#0050	Sets VRAM address to read a byte or more
void Bios_SetAddressForRead(u16 addr) __FASTCALL;

// SETWRT	#0053	Sets VRAM address to write or more
void Bios_SetAddressForWrite(u16 addr) __FASTCALL;

// FILVRM	#0056	Fills the specified VRAM area with the same data
void Bios_FillVRAM(u16 addr, u16 length, u8 value);

// LDIRMV	#0059	Block transfer from VRAM to memory
void Bios_TransfertVRAMtoRAM(u16 vram, u16 ram, u16 length);

// LDIRVM	#005C	Block transfer from memory to VRAM
void Bios_TransfertRAMtoVRAM(u16 ram, u16 vram, u16 length);

// CHGMOD	#005F	Switches to given screen mode
void Bios_ChangeMode(u8 screen) __FASTCALL;

// CHGCLR	#0062	Changes the screen colors
void Bios_ChangeColor(u8 text, u8 back, u8 border);

// NMI		#0066	Executes non-maskable interupt handling routine
// CLRSPR	#0069	Initialises all sprites

// INITXT	#006C	Switches to SCREEN 0 (text screen with 40×24 characters)
inline void Bios_InitScreen0();
inline void Bios_InitScreen0Ex(u16 pnt, u16 pgt, u8 width, u8 text, u8 bg, u8 border);
#define Bios_InitTextMode   Bios_InitScreen0
#define Bios_InitTextModeEx Bios_InitScreen0Ex

// INIT32	#006F	Switches to SCREEN 1 (text screen with 32×24 characters)
inline void Bios_InitScreen1();
inline void Bios_InitScreen1Ex(u16 pnt, u16 ct, u16 pgt, u16 sat, u16 sgt, u8 text, u8 bg, u8 border);
#define Bios_InitText32Mode   Bios_InitScreen1	
#define Bios_InitText32ModeEx Bios_InitScreen1Ex	

// INIGRP	#0072	Switches to SCREEN 2 (high resolution screen with 256×192 pixels)
inline void Bios_InitScreen2();
inline void Bios_InitScreen2Ex(u16 pnt, u16 ct, u16 pgt, u16 sat, u16 sgt, u8 text, u8 bg, u8 border);
#define Bios_InitGraphicMode   Bios_InitScreen2	
#define Bios_InitGraphicModeEx Bios_InitScreen2Ex	

// INIMLT	#0075	Switches to SCREEN 3 (multi-color screen with 64×48 pixels)
inline void Bios_InitScreen3();
inline void Bios_InitScreen3Ex(u16 pnt, u16 ct, u16 pgt, u16 sat, u16 sgt, u8 text, u8 bg, u8 border);
#define Bios_InitMulticolorMode   Bios_InitScreen3
#define Bios_InitMulticolorModeEx Bios_InitScreen3Ex

// SETTXT	#0078	Switches VDP to SCREEN 0 mode
inline void Bios_SetScreen0();
#define Bios_SetTextMode Bios_SetScreen0

// SETT32	#007B	Switches VDP to SCREEN 1 mode
inline void Bios_SetScreen1();
#define Bios_SetText32Mode Bios_SetScreen1	

// SETGRP	#007E	Switches VDP to SCREEN 2 mode
inline void Bios_SetScreen2();
#define Bios_SetGraphicMode Bios_SetScreen2	

// SETMLT	#0081	Switches VDP to SCREEN 3 mode
inline void Bios_SetScreen3();
#define Bios_SetMulticolorMode Bios_SetScreen3

// CALPAT	#0084	Returns the address of the sprite pattern table
u16 Bios_GetPatternTableAddress(u8 id) __FASTCALL;

// CALATR	#0087	Returns the address of the sprite attribute table
u16 Bios_GetAttributeTableAddress(u8 id) __FASTCALL;

// GSPSIZ	#008A	Returns current sprite size
u8 Bios_GetSpriteSize();

// GRPPRT	#008D	Displays a character on the graphic screen
void Bios_GraphicPrint(u8 chr) __FASTCALL;
void Bios_GraphicPrintEx(u8 chr, u16 x, u8 y, u8 op);

#endif // USE_BIOS_VDP

//=============================================================================
// PSG routines
//=============================================================================
#if USE_BIOS_PSG

// GICINI	#0090	Initialises PSG and sets initial value for the PLAY statement
inline void Bios_InitPSG();

// WRTPSG	#0093	Writes data to PSG register
void Bios_WritePSG(u8 reg, u8 value);

// RDPSG	#0096	Reads value from PSG register
u8 Bios_ReadPSG(u8 reg) __FASTCALL;

// STRTMS	#0099	Tests whether the PLAY statement is being executed as a background task. If not, begins to execute the PLAY statement
inline void Bios_PlayPSG();

#endif // USE_BIOS_PSG

//=============================================================================
// Console routines
//=============================================================================

// CHSNS	#009C	Tests the status of the keyboard buffer

// CHGET	#009F	Waits for a character input on the keyboard
u8 Bios_GetCharacter();

// CHPUT	#00A2	Displays one character
// LPTOUT	#00A5	Sends one character to printer
// LPTSTT	#00A8	Tests printer status
// CNVCHR	#00AB	Tests for the graphic header and transforms the code
// PINLIN	#00AE	Stores in the specified buffer the character codes input until the return key or STOP key is pressed
// INLIN	#00B1	Same as PINLIN except that AUGFLG (#F6AA) is set
// QINLIN	#00B4	Prints a question mark and one space
// BREAKX	#00B7	Tests status of CTRL-STOP
// ISCNTC	#00BA	Tests status of SHIFT-STOP
// CKCNTC	#00BD	Same as ISCNTC. used in Basic

// BEEP		#00C0	Generates beep
inline void Bios_Beep();

// CLS		#00C3	Clears the screen
inline void Bios_ClearScreen();

// POSIT	#00C6	Moves cursor to the specified position
// FNKSB	#00C9	Tests whether the function key display is active (FNKFLG)
// ERAFNK	#00CC	Erase functionkey display
// DSPFNK	#00CF	Displays the function keys
// TOTEXT	#00D2	Forces the screen to be in the text mode

//=============================================================================
// Controller routines
//=============================================================================

// GTSTCK	#00D5	Returns the joystick status
u8 Bios_GetJoystickDirection(u8 port) __FASTCALL;

// GTTRIG	#00D8	Returns current trigger status
u8 Bios_GetJoystickTrigger(u8 trigger) __FASTCALL;

// GTPAD	#00DB	Returns current touch pad status
// GTPDL	#00DE	Returns current value of paddle

//=============================================================================
// Tape device routines
//=============================================================================

// TAPION	#00E1	Reads the header block after turning the cassette motor on
// TAPIN	#00E4	Read data from the tape
// TAPIOF	#00E7	Stops reading from the tape
// TAPOON	#00EA	Turns on the cassette motor and writes the header
// TAPOUT	#00ED	Writes data on the tape
// TAPOOF	#00F0	Stops writing on the tape
// STMOTR	#00F3	Sets the cassette motor action

//=============================================================================
// Queue routines
//=============================================================================

// LFTQ		#00F6	Gives number of bytes in queue
// PUTQ		#00F9	Put byte in queue

//=============================================================================
// Graphic routines
//=============================================================================

// RIGHTC	#00FC	Shifts screen pixel to the right
// LEFTC	#00FF	Shifts screen pixel to the left
// UPC		#0102	Shifts screen pixel up
// TUPC		#0105	Tests whether UPC is possible, if possible, execute UPC
// DOWNC	#0108	Shifts screen pixel down
// TDOWNC	#010B	Tests whether DOWNC is possible, if possible, execute DOWNC
// SCALXY	#010E	Scales X and Y coordinates
// MAPXY	#0111	Places cursor at current cursor address
// FETCHC	#0114	Gets current cursor addresses mask pattern
// STOREC	#0117	Record current cursor addresses mask pattern
// SETATR	#011A	Set attribute byte
// READC	#011D	Reads attribute byte of current screen pixel
// SETC		#0120	Returns current screen pixel of specified attribute byte
// NSETCX	#0123	Set horizontal screen pixels
// GTASPC	#0126	Gets screen relations
// PNTINI	#0129	Initalises the PAINT instruction
// SCANR	#012C	Scans screen pixels to the right
// SCANL	#012F	Scans screen pixels to the left

//=============================================================================
// Misc routines
//=============================================================================

// CHGCAP	#0132	Alternates the CAPS lamp status
// CHGSND	#0135	Alternates the 1-bit sound port status
// RSLREG	#0138	Reads the primary slot register
// WSLREG	#013B	Writes value to the primary slot register
// RDVDP	#013E	Reads VDP status register

// SNSMAT	#0141	Returns the value of the specified line from the keyboard matrix
u8 Bios_GetKeyboardMatrix(u8 line) __FASTCALL;

// PHYDIO	#0144	Executes I/O for mass-storage media like disks
// FORMAT	#0147	Initialises mass-storage media like formatting of disks
// ISFLIO	#014A	Tests if I/O to device is taking place
// OUTDLP	#014D	Printer output
// GETVCP	#0150	Returns pointer to play queue
// GETVC2	#0153	Returns pointer to variable in queue number VOICEN (byte at #FB38)
// KILBUF	#0156	Clear keyboard buffer
// CALBAS	#0159	Executes inter-slot call to the routine in BASIC interpreter

//=============================================================================
// MSX 2
//=============================================================================
#if (MSX_VERSION >= MSX_2)

// SUBROM	#015C	Calls a routine in SUB-ROM
// EXTROM	#015F	Calls a routine in SUB-ROM. Most common way
// CHKSLZ	#0162	Search slots for SUB-ROM
// CHKNEW	#0165	Tests screen mode
// EOL		#0168	Deletes to the end of the line
// BIGFIL	#016B	Same function as FILVRM (with 16-bit VRAM-address)
// NSETRD	#016E	Same function as SETRD (with 16-bit VRAM-address)
// NSTWRT	#0171	Same function as SETWRT (with 16-bit VRAM-address)
// NRDVRM	#0174	Reads VRAM like in RDVRM (with 16-bit VRAM-address)
// NWRVRM	#0177	Writes to VRAM like in WRTVRM (with 16-bit VRAM-address)

#endif // (MSX_VERSION >= MSX_2)

//=============================================================================
// MSX 2+
//=============================================================================
#if (MSX_VERSION >= MSX_2Plus)

// RDRES	#017A	Read value of I/O port #F4
// WRRES	#017D	Write value to I/O port #F4

#endif // (MSX_VERSION >= MSX_2Plus)

//=============================================================================
// MSX turbo R
//=============================================================================
#if (MSX_VERSION == MSX_TurboR)

#define CPU_MODE_Z80		0x00
#define CPU_MODE_R800_ROM	0x01
#define CPU_MODE_R800_DRAM	0x02
#define CPU_TURBO_LED		0x80

// CHGCPU	#0180	Changes CPU mode
void Bios_SetCPUMode(u8 mode) __FASTCALL;
// GETCPU	#0183	Returns current CPU mode
u8 Bios_GetCPUMode();

// PCMPLY	#0186	Plays specified memory area through the PCM chip
// PCMREC	#0189	Records audio using the PCM chip into the specified memory area

#endif // (MSX_VERSION == MSX_TurboR)
