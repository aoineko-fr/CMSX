if /I %Target%==BIN (

	echo » Target: BASIC binary program ^(8000h~^)

	set Crt0=crt0_basic
	set StartAddr=8000
	set CodeAddr=8020
	set DataAddr=0
	set Ext=bin
	set FillSize=0
	set EmulParam=-diska .\emul\dsk
)
if /I %Target%==ROM16 (

	echo » Target: 16KB ROM in page 1 ^(4000h ~ 7FFFh^)

	set Crt0=crt0_rom16
	set StartAddr=4000
	set ROMSize=4000
	set CodeAddr=4010
	set DataAddr=8000
	set Ext=rom
	set FillSize=16384
	set EmulParam=-carta .\emul\%ProjName%.rom
)
if /I %Target%==ROM16P2 (

	echo » Target: 16KB ROM in page 2 ^(8000h ~ 7FFFh^)

	set Crt0=crt0_rom16p2
	set StartAddr=8000
	set ROMSize=4000
	set CodeAddr=8010
	set DataAddr=C000
	set Ext=rom
	set FillSize=16384
	set EmulParam=-carta .\emul\%ProjName%.rom
)
if /I %Target%==ROM32 (

	echo » Target: 32KB ROM in page 1^&2 ^(4000h ~ BFFFh^)

	set Crt0=crt0_rom32
	set StartAddr=4000
	set ROMSize=8000
	set CodeAddr=4010
	set DataAddr=C000
	set Ext=rom
	set FillSize=32768
	set EmulParam=-carta .\emul\%ProjName%.rom
)
if /I %Target%==ROM48 (

	echo » Target: 48KB ROM in page 0-2 ^(0000h ~ BFFFh^)

	set Crt0=crt0_rom48
	set StartAddr=0000
	set ROMSize=C000
	set CodeAddr=4000
	set DataAddr=C000
	set Ext=rom
	set FillSize=49152
	set EmulParam=-carta .\emul\%ProjName%.rom
)
if /I %Target%==DOS (

	echo » Target: MSX-DOS program ^(starting at 0100h^)

	set Crt0=crt0_dos
	set StartAddr=0100
	set CodeAddr=0108
	set DataAddr=0
	set Ext=com
	set FillSize=0
	set EmulParam=-diska .\emul\dos -ext msxdos2
)
if /I %Target%==DOSARG (

	echo » Target: MSX-DOS program with command line arguments ^(starting at 0100h^)

	set Crt0=crt0_dosarg
	set StartAddr=0100
	set CodeAddr=0180
	set DataAddr=0
	set Ext=com
	set FillSize=0
	set EmulParam=-diska .\emul\dos -ext msxdos2
)
