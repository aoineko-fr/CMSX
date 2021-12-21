@echo off
REM %~1 - expands %1 removing any surrounding quotes (")
REM %~f1 - expands %1 to a fully qualified path name
REM %~d1 - expands %1 to a drive letter only
REM %~p1 - expands %1 to a path only
REM %~n1 - expands %1 to a file name only
REM %~x1 - expands %1 to a file extension only
REM %~s1 - expanded path contains short names only
REM %~a1 - expands %1 to file attributes
REM %~t1 - expands %1 to date/time of file
REM %~z1 - expands %1 to size of file

set File=%~f1
set FilePath=%~d1%~p1
set FileName=%~n1
set FileExt=%~x1

if /I %Ext%==bin ( set TargetType=TARGET_TYPE_BIN)
if /I %Ext%==rom ( set TargetType=TARGET_TYPE_ROM)
if /I %Ext%==com ( set TargetType=TARGET_TYPE_DOS)

if not exist %OutDir% ( md %OutDir% )

if /I %Optim%==Speed (set CompileOpt=%CompileOpt% --opt-code-speed)
if /I %Optim%==Size (set CompileOpt=%CompileOpt% --opt-code-size)

set SDCCParam=-c -mz80 --vc -DTARGET=TARGET_%Target% -DTARGET_TYPE=%TargetType% -DMSX_VERSION=MSX_%Version% -I%ProjDir% -I%LibDir%\src %CompileOpt% --constseg RODATA %File% -o %OutDir%\
set ASMParam=-o -l -s -I%ProjDir% -I%LibDir%\src %File%

if /I %FileExt%==.c (
	echo %BLUE%Compiling %1 using SDCC C compiler...%RESET%
	
	echo SDCC %SDCCParam%
	%SDCC%\sdcc.exe %SDCCParam%
    if errorlevel 1 ( goto :Error )
)

if /I %FileExt%==.asm (
	echo %BLUE%Compiling %1 using SDASZ80 ASM compiler...%RESET%
	
	echo SDASZ80 %ASMParam%
    %SDCC%\sdasz80.exe %ASMParam%
    if errorlevel 1 ( goto :Error )
	move %FilePath%%FileName%.rel %OutDir%
	move %FilePath%%FileName%.lst %OutDir%
	move %FilePath%%FileName%.sym %OutDir%
)

echo %GREEN%Succeed%RESET%

exit /b %errorlevel%

:Error

echo %RED%Compile failed with error:%errorlevel%%RESET%

exit /b %errorlevel%