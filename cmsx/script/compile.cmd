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

if /I %Ext%==bin (set TargetType=TARGET_TYPE_BIN)
if /I %Ext%==rom (set TargetType=TARGET_TYPE_ROM)
if /I %Ext%==com (set TargetType=TARGET_TYPE_DOS)

if not exist %OutDir% ( md %OutDir% )

REM set SDCCParam=-c -mz80 --vc -DTARGET=TARGET_%Target% -DTARGET_TYPE=%TargetType% -I%ProjDir% -I%LibDir%\src --opt-code-speed --max-allocs-per-node20000 %File% -o %OutDir%\
set SDCCParam=-c -mz80 --vc -DTARGET=TARGET_%Target% -DTARGET_TYPE=%TargetType% -I%ProjDir% -I%LibDir%\src --opt-code-speed --constseg RODATA %File% -o %OutDir%\
set ASMParam=-o -l -s -I%ProjDir% -I%LibDir%\src %File%

if /I %FileExt%==.c (
	echo [94mCompiling %1 using SDCC C compiler...[0m
	
	echo SDCC %SDCCParam%
	%SDCC% %SDCCParam%
    if errorlevel 1 ( goto :Error )
)

if /I %FileExt%==.asm (
	echo [94mCompiling %1 using SDASZ80 ASM compiler...[0m
	
	echo SDASZ80 %ASMParam%
    %SDASZ80% %ASMParam%
    if errorlevel 1 ( goto :Error )
	move %FilePath%%FileName%.rel %OutDir%
	move %FilePath%%FileName%.lst %OutDir%
	move %FilePath%%FileName%.sym %OutDir%
)

echo [92mSucceed![0m

exit /b %errorlevel%

:Error

echo [91mFailed! Error:%errorlevel%[0m

exit /b %errorlevel%