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

setlocal EnableDelayedExpansion

rem ***************************************************************************
rem * TARGET SETTINGS                                                         *
rem ***************************************************************************
call %LibDir%\script\target_config.cmd


rem ***************************************************************************
rem * MODULES                                                                 *
rem ***************************************************************************

rem  Add crt0 source to build list (it must be the first in the list)
set SrcList=%LibDir%\src\crt0\%Crt0%.asm
set LibList=%OutDir%\%Crt0%.rel 

rem  Add project source to build list
set SrcList= %SrcList%,%ProjName%.c
set LibList=%LibList% %OutDir%\%ProjName%.rel

rem  Add modules sources to build list
for %%G in (%ModuleList%) do (
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

if %Optim%==Speed (set LinkOpt=%LinkOpt% --opt-code-speed)
if %Optim%==Size (set LinkOpt=%LinkOpt% --opt-code-size)

set SDCCParam=-mz80 --no-std-crt0 --code-loc 0x%CodeAddr% --data-loc 0x%DataAddr% --constseg RODATA --vc %LinkOpt% %LibList% -o %OutDir%\
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
REM START /b %DEBUGGER%

:NoRun

REM ////////////////////////////////////////

echo [92mBuild Succeed![0m
exit /b %errorlevel%

:Error

echo [91mBuild Failed! Error:%errorlevel%[0m
exit /b %errorlevel%