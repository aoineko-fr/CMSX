@echo off

rem ***************************************************************************
rem * DIRECTORIES SETTINGS                                                    *
rem ***************************************************************************
set ProjDir=.
REM set ProjDir=%~d0%~p0
set OutDir=%ProjDir%\out
set LibDir=%ProjDir%\..\..\cmsx
set ToolsDir=%ProjDir%\..\..\tools
rem ***************************************************************************
rem * TOOLS SETTINGS                                                          *
rem ***************************************************************************
set SDCC=%ToolsDir%\SDCC\bin
set Hex2Bin=%ToolsDir%\Hex2bin\hex2bin.exe
set FillFile=%ToolsDir%\MakeROM\fillfile.exe
set Emulator=%ToolsDir%\OpenMSX\openmsx.exe
set Debugger=%ToolsDir%\OpenMSX\Debugger\openmsx-debugger.exe
set MSXDOS=%ToolsDir%\MSXDOS
set DskTool=%ToolsDir%\DskTool\dsktool.exe

rem ***************************************************************************
rem * PROJECT SETTINGS                                                        *
rem ***************************************************************************

rem  Project name (will be use for output filename)
set ProjName=%~n1

rem  Project modules to build (use ProjName if not defined)
set ProjModules=%ProjName%

rem  List of library modules to build
set LibModules=system,bios,vdp,print,input,memory,math,draw

rem  MSX version:
rem  - 1		MSX 1
rem  - 2		MSX 2
rem  - 2P		MSX 2+
rem  - TR		MSX TurboR
rem  - 12		MSX 1/2
set Version=2

rem  Target:
rem  - BIN				.bin	BASIC binary program (8000h~)
rem  - ROM_8K			.rom	8KB ROM in page 1 (4000h ~ 5FFFh)
rem  - ROM_8K_P2		.rom	8KB ROM in page 2 (8000h ~ 9FFFh)
rem  - ROM_16K			.rom	16KB ROM in page 1 (4000h ~ 7FFFh)
rem  - ROM_16K_P2		.rom	16KB ROM in page 2 (8000h ~ BFFFh)
rem  - ROM_32K			.rom	32KB ROM in page 1-2 (4000h ~ BFFFh)
rem  - ROM_32K_P0		.rom	32KB ROM in page 0-1 (0000h ~ 7FFFh)
rem  - ROM_48K			.rom	48KB ROM in page 0-2 (0000h ~ BFFFh). Pages 1-2 visible at start
rem  - ROM_48K_ISR		.rom	48KB ROM in page 0-2 (0000h ~ BFFFh). Pages 0-2 visible at start
rem  - ROM_64K			.rom	64KB ROM in page 0-3 (0000h ~ FFFFh). Pages 1-2 visible at start
rem  - ROM_64K_ISR		.rom	64KB ROM in page 0-3 (0000h ~ FFFFh). Pages 0-2 visible at start
rem  - ROM_ASCII8		.rom	128KB ROM using ASCII-8 mapper
rem  - ROM_ASCII16		.rom	128KB ROM using ASCII-16 mapper
rem  - ROM_KONAMI		.rom	128KB ROM using Konami mapper (8KB segments)
rem  - ROM_KONAMI_SCC	.rom	128KB ROM using Konami SCC mapper (8KB segments)
rem  - DOS				.com	MSX-DOS program (0100h~) No direct acces to Main-ROM
rem  - DOS_ARG			.com	MSX-DOS program (using command line arguments ; 0100h~) No direct acces to Main-ROM
set Target=ROM_32K

rem  Optim:
rem  - Default
rem  - Speed
rem  - Size
set Optim=Speed

rem  Overwrite RAM starting address
set ForceRamAddr=

rem  Additionnal compilation flag
set CompileOpt=

rem  Verbose mode: 0 or 1
set Verbose=0

rem  Emulator options: 0 or 1
set EmulMachine=0
set Emul60Hz=0
set EmulFullScreen=0
set EmulMute=0
set EmulSubSlot=1
set EmulDebug=0
rem  Emulator extra parameters to be add to command-line
set EmulExtraParam=

rem  Check for project specific parameters
if exist %ProjName%.cmd do (
	call %ProjName%.cmd
)

rem ***************************************************************************
rem * BUILD STEPS                                                             *
rem ***************************************************************************
set DoClean=0
set DoCompile=1
set DoMake=1
set DoPackage=1
set DoDeploy=1
set DoRun=0

rem ***************************************************************************
rem * START BUILD                                                             *
rem ***************************************************************************
call %LibDir%\script\build.cmd