@echo off

for %%G in (%SrcList%) do (
	call %LibDir%\script\compile.cmd %%G
    if errorlevel 1 (exit /b 1)
)

exit /b 0