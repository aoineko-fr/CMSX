@ECHO off

rem ***************************************************************************
rem * DIRECTORIES SETTINGS                                                    *
rem ***************************************************************************
set ProjDir=.
set OutDir=.\out
set LibDir=..\..\cmsx
set ToolsDir=..\..\tools

rem ***************************************************************************
rem * TOOLS SETTINGS                                                          *
rem ***************************************************************************
set SDCC=%ToolsDir%\SDCC400\bin\sdcc.exe
set SDASZ80=%ToolsDir%\SDCC400\bin\sdasz80.exe
set HEX2BIN=%ToolsDir%\Hex2bin\hex2bin.exe
set FILLFILE=%ToolsDir%\MakeROM\fillfile.exe
set EMUL=%ToolsDir%\OpenMSX\openmsx.exe
set DEBUGGER=%ToolsDir%\OpenMSX\Debugger\openmsx-debugger.exe
set MSXDOS=%ToolsDir%\MSXDOS

rem ***************************************************************************
rem * PROJECT SETTINGS                                                        *
rem ***************************************************************************
SET ProjName=sample
set SrcList=sample.c,%LibDir%\src\bios_main.c,%LibDir%\src\video.c,%LibDir%\src\font.c,%LibDir%\src\call_hl.s,%LibDir%\src\msxi\msxi_unpack.c
set LibList=%OutDir%\sample.rel %OutDir%\bios_main.rel %OutDir%\video.rel %OutDir%\font.rel %OutDir%\call_hl.rel %OutDir%\msxi_unpack.rel
rem  Target:
rem  - BIN		.bin	BASIC binary program (8000h~)
rem  - ROM16	.rom	16KB ROM in page 1 (4000h ~ 7FFFh)
rem  - ROM16P2	.rom	16KB ROM in page 2 (8000h ~ 7FFFh)
rem  - ROM32	.rom	32KB ROM in page 1&2 (4000h ~ BFFFh)
rem  - ROM48	.rom	48KB ROM in page 0-2 (0000h ~ BFFFh) No direct acces to Main-ROM @todo To be implemented!
rem  - DOS		.com	MSX-DOS program (0100h~) No direct acces to Main-ROM
rem  - DOSARG	.com	MSX-DOS program (using command line arguments ; 0100h~) No direct acces to Main-ROM
set Target=DOSARG
rem  Optim:
rem  - Default
rem  - Speed
rem  - Size
set Optim=Speed

rem ***************************************************************************
rem * BUILD STEPS                                                             *
rem ***************************************************************************
set DoClean=0
set DoCompile=1
set DoMake=1
set DoPackage=1
set DoDeploy=1
set DoRun=1

rem ***************************************************************************
rem * START BUILD                                                             *
rem ***************************************************************************
call %LibDir%\script\build.cmd