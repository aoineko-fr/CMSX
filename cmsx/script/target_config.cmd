set MapperSize=0

rem ---------------------------------------------------------------------------
if /I %Target%==BIN (

	echo » Target: BASIC binary program ^(8000h~^)

	set Ext=bin
	set Crt0=crt0_basic
	set StartAddr=8000
	set CodeAddr=8007
	set RamAddr=0
	set FillSize=0
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_16K (

	echo » Target: 16KB ROM in page 1 ^(4000h ~ 7FFFh^)

	set Ext=rom
	set Crt0=crt0_rom16
	set StartAddr=4000
	set CodeAddr=4010
	set RamAddr=8000
	set /A FillSize=16*1024
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_16K_P2 (

	echo » Target: 16KB ROM in page 2 ^(8000h ~ 7FFFh^)

	set Ext=rom
	set Crt0=crt0_rom16p2
	set StartAddr=8000
	set CodeAddr=8010
	set RamAddr=C000
	set /A FillSize=16*1024
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_32K (

	echo » Target: 32KB ROM in page 1^&2 ^(4000h ~ BFFFh^)

	set Ext=rom
	set Crt0=crt0_rom32
	set StartAddr=4000
	set CodeAddr=4010
	set RamAddr=C000
	set /A FillSize=32*1024
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_48K (

	echo » Target: 48KB ROM in page 0-2 ^(0000h ~ BFFFh^)

	set Ext=rom
	set Crt0=crt0_rom48
	set StartAddr=0000
	set CodeAddr=4000
	set RamAddr=C000
	set /A FillSize=48*1024
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_48K_ISR (

	echo » Target: 48KB ROM in page 0-2 ^(0000h ~ BFFFh^) with ISR replacement

	set Ext=rom
	set Crt0=crt0_rom48_isr
	set StartAddr=0000
	set CodeAddr=4000
	set RamAddr=C000
	set /A FillSize=48*1024
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_64K_ISR (

	echo » Target: 64KB ROM in page 0-3 ^(0000h ~ FFFFh^) with ISR replacement

	set Ext=rom
	set Crt0=crt0_rom48_isr
	set StartAddr=0000
	set CodeAddr=4000
	set RamAddr=C000
	set /A FillSize=64*1024
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_ASCII8 (

	echo » Target: 128KB ROM using ASCII-8 mapper ^(starting at 4000h^)

	set Ext=rom
	set Crt0=crt0_rom_mapper
	set StartAddr=4000
	set CodeAddr=4010
	set RamAddr=C000
	set /A FillSize=32*1024
	set /A MapperSize=128*1024
	set /A SegSize=8*1024
	set Bank0Addr=4000
	set Bank1Addr=6000
	set Bank2Addr=8000
	set Bank3Addr=A000
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_ASCII16 (

	echo » Target: 128KB ROM using ASCII-16 mapper ^(starting at 4000h^)

	set Ext=rom
	set Crt0=crt0_rom_mapper
	set StartAddr=4000
	set CodeAddr=4010
	set RamAddr=C000
	set /A FillSize=32*1024
	set /A MapperSize=128*1024
	set /A SegSize=16*1024
	set Bank0Addr=6000
	set Bank1Addr=77FF
	set Bank2Addr=0
	set Bank3Addr=0
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_KONAMI (

	echo » Target: 128KB ROM using KONAMI mapper ^(starting at 4000h^)

	set Ext=rom
	set Crt0=crt0_rom_mapper
	set StartAddr=4000
	set CodeAddr=4010
	set RamAddr=C000
	set /A FillSize=32*1024
	set /A MapperSize=128*1024
	set /A SegSize=8*1024
	set Bank0Addr=0
	set Bank1Addr=6000
	set Bank2Addr=8000
	set Bank3Addr=A000
)
rem ---------------------------------------------------------------------------
if /I %Target%==ROM_KONAMI_SCC (

	echo » Target: 128KB ROM using KONAMI SCC mapper ^(starting at 4000h^)

	set Ext=rom
	set Crt0=crt0_rom_mapper
	set StartAddr=4000
	set CodeAddr=4010
	set RamAddr=C000
	set /A FillSize=32*1024
	set /A MapperSize=128*1024
	set /A SegSize=8*1024
	set Bank0Addr=5000
	set Bank1Addr=7000
	set Bank2Addr=9000
	set Bank3Addr=B000
)
rem ---------------------------------------------------------------------------
if /I %Target%==DOS (

	echo » Target: MSX-DOS program ^(starting at 0100h^)

	set Ext=com
	set Crt0=crt0_dos
	set StartAddr=0100
	set CodeAddr=0100
	set RamAddr=0
	set FillSize=0
)
rem ---------------------------------------------------------------------------
if /I %Target%==DOS_ARG (

	echo » Target: MSX-DOS program with command line arguments ^(starting at 0100h^)

	set Ext=com
	set Crt0=crt0_dosarg
	set StartAddr=0100
	set CodeAddr=0180
	set RamAddr=0
	set FillSize=0
)
