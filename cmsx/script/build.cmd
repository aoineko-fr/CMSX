@echo off
"%__APPDIR__%chcp.com" 65001 > nul
title <nul & title CMSX Build Tool – %ProjName% – %Target%

setlocal EnableDelayedExpansion

rem -- Setup text color
set RESET=[0m
set RED=[91m
set GREEN=[92m
set YELLOW=[93m
set BLUE=[94m
set MAGENTA=[95m
set CYAN=[96m
set BG=[44m

cls
echo %BG%
echo ╔═══════════════════════════════════════════════════════════════════════════╗
echo ║                                                                           ║
echo ║  ██▀█▀██▀▀▀█▀▀█▀█  ▄▄▄ ▄▄                                                 ║
echo ║  █  ▄ █▄ ▀██▄ ▀▄█ ██   ██                                                 ║
echo ║  █  █ █▀▀ ▄█  █ █ ▀█▄█ ██▄▄                                               ║
echo ║  ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀                                                         ║
echo ║   ▄▄▄       ▄  ▄▄    ▄▄   ▄▄▄▄           ▄▄                               ║
echo ║   ██▄▀ ██ █ ▄  ██   ▄██    ██  ▄█▀▄ ▄█▀▄ ██                               ║
echo ║   ██▄▀ ▀█▄█ ██ ▀█▄ ▀▄██    ██  ▀█▄▀ ▀█▄▀ ▀█▄                              ║
echo ║                                                                           ║
echo ╚═══════════════════════════════════════════════════════════════════════════╝
echo %RESET%

rem ***************************************************************************
rem * TARGET SETTINGS                                                         *
rem ***************************************************************************

rem -- Check MSX version
if /I %Version%==1			( echo » Version: MSX 1
) else if /I %Version%==2	( echo » Version: MSX 2
) else if /I %Version%==2P	( echo » Version: MSX 2+
) else if /I %Version%==TR	( echo » Version: MSX turbo R
) else if /I %Version%==12	( echo » Version: MSX 1/2
) else (
	echo %RED%Error: Unknow MSX Version%RESET%
	exit /b 10
)

rem -- Target specific initializations
call %LibDir%\script\target_config.cmd

rem -- Overwrite RAM start address
if defined %ForceDataAddr% do (
	echo Force RAM address to %ForceDataAddr%
	set DataAddr=%ForceDataAddr%
)

rem ***************************************************************************
rem * CHECK PARAMETERS                                                        *
rem ***************************************************************************

rem -- Check tools
if not exist %SDCC%\sdcc.exe (
	echo %RED%Error: Invalide path to C Compiler [%SDCC%\sdcc.exe]%RESET%
	exit /b 20
)
if not exist %SDCC%\sdasz80.exe (
	echo %RED%Error: Invalide path to ASM Compiler [%SDCC%\sdasz80.exe]%RESET%
	exit /b 20
)
if not exist %Hex2Bin% (
	echo %RED%Error: Invalide path to Hex2Bin [%Hex2Bin%]%RESET%
	exit /b 20
)
if not exist %FillFile% (
	echo %RED%Error: Invalide path to FillFile [%FillFile%]%RESET%
	exit /b 20
)
if not exist %Emulator% (
	echo %YELLOW%Warning: Invalide path to Emulator [%Emulator%]%RESET%
)
	
if not exist %Debugger% (
	echo %YELLOW%Warning: Invalide path to Debugger [%Debugger%]%RESET%
)

rem ***************************************************************************
rem * MODULES                                                                 *
rem ***************************************************************************

rem  Add crt0 source to build list (it must be the first in the list)
set SrcList=%LibDir%\src\crt0\%Crt0%.asm
set LibList=%OutDir%\%Crt0%.rel 

rem  Add project sources to build list
for %%G in (%ProjName%) do (
	if not exist %%G.c (
		echo %RED%Error: Source file %%G.c don't exist%RESET%
		exit /b 20
	)
	set SrcList=!SrcList!,%%G.c
	set LibList=!LibList! %OutDir%\%%~nG.rel
)

echo » Modules: %ModuleList%

rem  Add modules sources to build list
for %%G in (%ModuleList%) do (
	if not exist %LibDir%\src\%%G.c (
		echo %RED%Error: Module %%G don't exist%RESET%
		exit /b 30
	)
	set SrcList=!SrcList!,%LibDir%\src\%%G.c
	set LibList=!LibList! %OutDir%\%%~nG.rel
)

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

%SDCC%\sdcc.exe --version

call %LibDir%\script\compile_all.cmd
if errorlevel 1 goto :Error

:NoCompile

rem ***************************************************************************
if %DoMake%==0 goto :NoMake

echo.
echo ┌───────────────────────────────────────────────────────────────────────────┐
echo │ MAKE                                                                      │
echo └───────────────────────────────────────────────────────────────────────────┘

echo %BLUE%Making %ProjName% using SDCC...%RESET%

%SDCC%\sdcc.exe --version

if %Optim%==Speed (set LinkOpt=%LinkOpt% --opt-code-speed)
if %Optim%==Size (set LinkOpt=%LinkOpt% --opt-code-size)

set SDCCParam=-mz80 --no-std-crt0 --code-loc 0x%CodeAddr% --data-loc 0x%DataAddr% --constseg RODATA --vc %LinkOpt% %LibList% -o %OutDir%\
echo SDCC %SDCCParam%
%SDCC%\sdcc.exe %SDCCParam%
if errorlevel 1 goto :Error
echo %GREEN%Succeed%RESET%

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
echo %BLUE%Converting to binary...%RESET%

echo HEX2BIN %H2BParam%
%Hex2Bin% %H2BParam% 
if errorlevel 1 goto :Error
echo %GREEN%Succeed%RESET%

rem ***************************************************************************
rem * FILL                                                                    *
rem ***************************************************************************
if %FillSize% EQU 0 goto :NoFill

echo %BLUE%Filling binary up to %FillSize% bytes...%RESET%
%FillFile% %OutDir%\%Crt0%.%Ext% %FillSize%
if errorlevel 1 goto :Error
echo %GREEN%Succeed%RESET%

:NoFill
:NoPackage

rem ***************************************************************************
if DoDeploy==0 goto :NoDeploy

echo.
echo ┌───────────────────────────────────────────────────────────────────────────┐
echo │ DEPLOY                                                                    │
echo └───────────────────────────────────────────────────────────────────────────┘

echo %BLUE%Deploying %Target%...%RESET%

if /I %Ext%==bin (
	echo Copy %OutDir%\%Crt0%.%Ext% to emul\dsk\%ProjName%.%Ext%
	copy %OutDir%\%Crt0%.%Ext% .\emul\dsk\%ProjName%.%Ext%
	if errorlevel 1 goto :Error
	echo Create emul\dsk\autoexec.bas
	echo 10 print"Loading..." > .\emul\dsk\autoexec.bas
	echo 20 bload"%ProjName:~0,8%.%Ext%",r >> .\emul\dsk\autoexec.bas
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
	echo echo Loading... > .\emul\dos\autoexec.bat
	echo %ProjName%.%Ext% >> .\emul\dos\autoexec.bat
	if errorlevel 1 goto :Error
)
echo %GREEN%Succeed%RESET%

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
START /b %Emulator% %EmulParam%

rem ***************************************************************************
rem * DEBUGGER                                                                *
rem ***************************************************************************
REM START /b %Debugger%

:NoRun

REM ////////////////////////////////////////

echo %GREEN%Build Succeed%RESET%
exit /b %errorlevel%

:Error

echo %RED%Build Failed with error:%errorlevel%%RESET%
exit /b %errorlevel%