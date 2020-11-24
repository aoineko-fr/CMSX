//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// I/O Ports
//
// References:
// - http://map.grauw.nl/resources/msx_io_ports.php
// - https://www.msx.org/wiki/I/O_Ports_List

//-----------------------------------------------------------------------------
// PPI


#define P_PSL_STAT #0xA8   // slot status
#define P_KBD_STAT #0xA9   // keyboard status
#define P_GIO_REGS #0xAA   // General IO Register
#define P_PPI_REGS #0xAB   // PPI register

#define P_PPI_A 0xA8
__sfr __at(P_PPI_A) g_PortPrimarySlot; // Port to access the primary Slot selection register. (PPI's port A is used)
// bit 0~1 = Number of slot to select on page 0 (0000h~3FFFh)
// bit 2~3 = Number of slot to select on page 1 (4000h~7FFFh)
// bit 4~5 = Number of slot to select on page 2 (8000h~BFFFh)
// bit 6~7 = Number of slot to select on page 3 (C000h~FFFFh)

#define P_PPI_B 0xA9
__sfr __at(P_PPI_B) g_PortReadKeyboard; // Port to read the keyboard matrix row specified via the port AAh. (PPI's port B is used)

#define P_PPI_C 0xAA
__sfr __at(P_PPI_C) g_PortAccessKeyboard; // Port to access the register that control the keyboard CAP LED, two signals to data recorder and a matrix row (use the port C of PPI).
// bits 0~3 = Row number of specified keyboard matrix to read via port B
// bit 4 = Data recorder motor (reset to turn on)
// bit 5 = Set to write on tape
// bit 6 = Keyboard LED CAPS (reset to turn on)
// bit 7 = 1, then 0 shortly thereafter to make a clicking sound (used for the keyboard).

#define P_PPI_MODE 0xAB
__sfr __at(P_PPI_MODE) g_PortControl; // Port to access the ports control register. (Write only)
// bit 0 = Bit status to change
// bit 1~3 = Number of the bit to change at port C of the PPI
// bit 4~6 = Unused
// bit 7 = Must be always reset on MSX.

//-----------------------------------------------------------------------------
// VDP

// #98	VRAM data read/write port
#define P_VDP_DATA #0x98   // VDP data port (VRAM read/write)
__sfr __at(P_VDP_DATA) g_PortVDPData; // Port to access the ports control register. (Write only)

// #99	(write) VDP register write port (bit 7=1 in second write)
// 		VRAM address register (bit 7=0 in second write, bit 6: read/write access (0=read))
#define P_VDP_ADDR #0x99   // VDP address (write only)
__sfr __at(P_VDP_ADDR) g_PortVDPAddr; // Port to access the ports control register. (Write only)

// #99	(read) Status register read port
#define P_VDP_STAT #0x99   // VDP status (read only)
__sfr __at(P_VDP_STAT) g_PortVDPStat; // Port to access the ports control register. (Write only)

// #9A	Palette access port (only v9938/v9958)
#define P_VDP_PALT #0x9A   // VDP palette latch (write only)
__sfr __at(P_VDP_PALT) g_PortVDPPal; // Port to access the ports control register. (Write only)

// #9B	Indirect register access port (only v9938/v9958)
#define P_VDP_REGS #0x9B   // VDP register access (write only)
__sfr __at(P_VDP_REGS) g_PortVDPReg; // Port to access the ports control register. (Write only)

//-----------------------------------------------------------------------------
// PSG

#define P_PSG_REGS 0xA0   // PSG register write port
#define P_PSG_DATA 0xA1   // PSG value write port
#define P_PSG_STAT 0xA2   // PSG value read port

//-----------------------------------------------------------------------------
// RTC

#define P_RTC_ADDR #0xB4   // RTC address
#define P_RTC_DATA #0xB5   // RTC data
