@echo off

setlocal EnableDelayedExpansion
	
set Mappers=BIN,DOS1,DOS2,ROM_8K,ROM_8K_P2,ROM_16K,ROM_16K_P2,ROM_32K,ROM_48K,ROM_48K_ISR,ROM_64K,ROM_64K_ISR,ROM_ASCII8,ROM_ASCII16,ROM_KONAMI,ROM_KONAMI_SCC
set Unsupported=ROM_32K_P0,DOS2_ARG

for %%G in (%Mappers%) do (

	rem ---- Build target ----
	call build %%G

	rem ---- Rename output file to prevent overwriting ----
	call :RenameOutput %%G
	
	REM pause
)

goto :EOF

:RenameOutput
	set Temp=%1
	set Type=%Temp:~0,3%
	if /I !Type!==bin (
		echo Rename %ProjDir%\emul\dsk\%ProjName%.dsk to %ProjName%_%1.dsk 
		if exist %ProjDir%\emul\dsk\%ProjName%_%1.dsk ( del /Q %ProjDir%\emul\dsk\%ProjName%_%1.dsk )
		rename %ProjDir%\emul\dsk\%ProjName%.dsk %ProjName%_%1.dsk 
	)
	if /I !Type!==rom (
		echo Rename %ProjDir%\emul\rom\%ProjName%.rom to %ProjName%_%1.rom 
		if exist %ProjDir%\emul\rom\%ProjName%_%1.rom ( del /Q %ProjDir%\emul\rom\%ProjName%_%1.rom )
		rename %ProjDir%\emul\rom\%ProjName%.rom %ProjName%_%1.rom
	)
	if /I !Type!==dos (
		echo Rename %ProjDir%\emul\dsk\%ProjName%.dsk to %ProjName%_%1.dsk 
		if exist %ProjDir%\emul\dsk\%ProjName%_%1.dsk ( del /Q %ProjDir%\emul\dsk\%ProjName%_%1.dsk )
		rename %ProjDir%\emul\dsk\%ProjName%.dsk %ProjName%_%1.dsk 
	)
	exit /B 0
	
:EOF
