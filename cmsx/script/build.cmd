@echo off
"%__APPDIR__%chcp.com" 65001 > nul
title <nul & title CMSX Build Tool – %ProjName% – %Target%
cls
echo ╔═══════════════════════════════════════════════════════════════════════════╗
echo ║ █▀▀ █▀▄▀█ █▀ ▀▄▀                                                          ║
echo ║ █▄▄ █ ▀ █ ▄█ █ █ v0.2                                                     ║
echo ╟───────────────────────────────────────────────────────────────────────────╢
echo ║ BUILD TOOL                                                                ║
echo ╚═══════════════════════════════════════════════════════════════════════════╝
echo.

rem ***************************************************************************
rem * TARGET SETTINGS                                                         *
rem ***************************************************************************
REM call %LibDir%\script\target_config.cmd
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

set SrcList=%LibDir%\src\crt0\%Crt0%.s %SrcList%
set LibList=%OutDir%\%Crt0%.rel %LibList%

rem ***************************************************************************
if %DoClean%==0 goto :NoClean

echo.
echo ┌───────────────────────────────────────────────────────────────────────────┐
echo │ CLEAN                                                                     │
echo └───────────────────────────────────────────────────────────────────────────┘

if exist %OutDir% (
	echo Removing %OutDir%...
	rd /S /Q %OutDir% 
)
if exist .\emul (
	echo Removing \emul...
	rd /S /Q .\emul
)

:NoClean

rem ***************************************************************************
rem * INIT                                                                    *
rem ***************************************************************************
if not exist %OutDir% ( md %OutDir% )
if not exist .\emul ( md .\emul )
if not exist .\emul\dsk ( md .\emul\dsk )
if not exist .\emul\dos (
	md .\emul\dos
	copy %MSXDOS%\*.* .\emul\dos
)

rem ***************************************************************************
if %DoCompile%==0 goto :NoCompile

echo.
echo ┌───────────────────────────────────────────────────────────────────────────┐
echo │ COMPILE                                                                   │
echo └───────────────────────────────────────────────────────────────────────────┘

call %LibDir%\script\compile_all.cmd
if errorlevel 1 goto :Error

:NoCompile

rem ***************************************************************************
if %DoMake%==0 goto :NoMake

echo.
echo ┌───────────────────────────────────────────────────────────────────────────┐
echo │ MAKE                                                                      │
echo └───────────────────────────────────────────────────────────────────────────┘

echo [94mMaking %ProjName% using SDCC...[0m

set SDCCParam=-mz80 --no-std-crt0 --code-loc 0x%CodeAddr% --data-loc 0x%DataAddr% --vc --opt-code-speed %LibList% -o %OutDir%\
echo SDCC %SDCCParam%
%SDCC% %SDCCParam%
if errorlevel 1 goto :Error
echo [92mSucceed![0m

:NoMake

rem ***************************************************************************
if %DoPackage%==0 goto :NoPackage

echo.
echo ┌───────────────────────────────────────────────────────────────────────────┐
echo │ PACKAGE                                                                   │
echo └───────────────────────────────────────────────────────────────────────────┘

rem ***************************************************************************
rem * HEX2BIN                                                                 *
rem ***************************************************************************
set H2BParam=-e %Ext% -s %StartAddr% %OutDir%\%Crt0%.ihx
echo [94mConverting to binary...[0m

echo HEX2BIN %H2BParam%
%HEX2BIN% %H2BParam% 
if errorlevel 1 goto :Error
echo [92mSucceed![0m

rem ***************************************************************************
rem * FILL                                                                    *
rem ***************************************************************************
if %FillSize% EQU 0 goto :NoFill

echo [94mFilling binary up to %FillSize% bytes...[0m
%FILLFILE% %OutDir%\%Crt0%.%Ext% %FillSize%
if errorlevel 1 goto :Error
echo [92mSucceed![0m

:NoFill
:NoPackage

rem ***************************************************************************
if DoDeploy==0 goto :NoDeploy

echo.
echo ┌───────────────────────────────────────────────────────────────────────────┐
echo │ DEPLOY                                                                    │
echo └───────────────────────────────────────────────────────────────────────────┘

echo [94mDeploying %Target%...[0m

if /I %Ext%==bin (
	echo Copy %OutDir%\%Crt0%.%Ext% to emul\dsk\%ProjName%.%Ext%
	copy %OutDir%\%Crt0%.%Ext% .\emul\dsk\%ProjName%.%Ext%
	if errorlevel 1 goto :Error
	echo Create emul\dsk\autoexec.bas
	echo 10 bload"%ProjName%.%Ext%",r > .\emul\dsk\autoexec.bas
	if errorlevel 1 goto :Error
)
if /I %Ext%==rom (
	echo Copy %OutDir%\%Crt0%.%Ext% to emul\%ProjName%.%Ext%
	copy %OutDir%\%Crt0%.%Ext% .\emul\%ProjName%.%Ext%
	if errorlevel 1 goto :Error
)
if /I %Ext%==com (
	echo Copy %OutDir%\%Crt0%.%Ext% to emul\dos\%ProjName%.%Ext%
	copy %OutDir%\%Crt0%.%Ext% .\emul\dos\%ProjName%.%Ext%
	if errorlevel 1 goto :Error
	echo Create emul\dos\autoexec.bat
	echo %ProjName%.%Ext% > .\emul\dos\autoexec.bat
	if errorlevel 1 goto :Error
)
echo [92mSucceed![0m

:NoDeploy

rem ***************************************************************************
if %DoRun%==0 goto :NoRun

echo.
echo ┌───────────────────────────────────────────────────────────────────────────┐
echo │ RUN                                                                       │
echo └───────────────────────────────────────────────────────────────────────────┘

rem ***************************************************************************
rem * EMULATOR                                                                *
rem ***************************************************************************
START /b %EMUL% %EmulParam%

rem ***************************************************************************
rem * DEBUGGER                                                                *
rem ***************************************************************************
START /b %DEBUGGER%

:NoRun

REM ////////////////////////////////////////

echo [92mBuild Succeed![0m
exit /b %errorlevel%

:Error

echo [91mBuild Failed! Error:%errorlevel%[0m
exit /b %errorlevel%