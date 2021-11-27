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
set SDCC=%ToolsDir%\SDCC4112\bin\sdcc.exe
set SDASZ80=%ToolsDir%\SDCC4112\bin\sdasz80.exe
set HEX2BIN=%ToolsDir%\Hex2bin\hex2bin.exe
set FILLFILE=%ToolsDir%\MakeROM\fillfile.exe
set EMUL=%ToolsDir%\OpenMSX\openmsx.exe
set DEBUGGER=%ToolsDir%\OpenMSX\Debugger\openmsx-debugger.exe
set MSXDOS=%ToolsDir%\MSXDOS

rem ***************************************************************************
rem * PROJECT SETTINGS                                                        *
rem ***************************************************************************
SET ProjName=%1
SET ModuleList=bios,vdp,print,input,memory,math,draw,clock,pt3\pt3_player,ayfx\ayfx_player
REM SET ModuleList=wyz\wyz_player,psg
rem  Target:
rem  - BIN		.bin	BASIC binary program (8000h~)
rem  - ROM16	.rom	16KB ROM in page 1 (4000h ~ 7FFFh)
rem  - ROM16P0	.rom	16KB ROM in page 0 (0000h ~ 3FFFh)
rem  - ROM16P2	.rom	16KB ROM in page 2 (8000h ~ BFFFh)
rem  - ROM32	.rom	32KB ROM in page 1-2 (4000h ~ BFFFh)
rem  - ROM32P0	.rom	32KB ROM in page 0-1 (0000h ~ 7FFFh)
rem  - ROM48	.rom	48KB ROM in page 0-2 (0000h ~ BFFFh) No direct acces to Main-ROM
rem  - DOS		.com	MSX-DOS program (0100h~) No direct acces to Main-ROM
rem  - DOSARG	.com	MSX-DOS program (using command line arguments ; 0100h~) No direct acces to Main-ROM
set Target=ROM32
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