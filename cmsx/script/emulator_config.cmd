for %%I in ("%Emulator%") do (set EmulatorName=%%~nI)

echo %BLUE%Starting %EmulatorName% emulator...%RESET%

echo EmulSetMachine=%EmulSetMachine%
echo Emul60Hz=%Emul60Hz%
echo EmulSetFullScreen=%EmulSetFullScreen%
echo EmulMute=%EmulMute%
echo EmulAddExpander=%EmulAddExpander%
echo EmulStartDebugger=%EmulStartDebugger%

set  EmulatorArgs=%EmulExtraParam%

rem ***************************************************************************
rem * OpenMSX                                                                 *
rem ***************************************************************************
rem  Doc: https://openmsx.org/manual/commands.html
if /I %EmulatorName%==openmsx (

	rem ---- Add launch options ----
	if %EmulSetMachine%==1 (
		if /I %Version%==1 (
			set EmulatorArgs=!EmulatorArgs! -machine C-BIOS_MSX1
		) else if /I %Version%==2 (
			set EmulatorArgs=!EmulatorArgs! -machine C-BIOS_MSX2
		) else if /I %Version%==2P (
			set EmulatorArgs=!EmulatorArgs! -machine C-BIOS_MSX2+
		) else if /I %Version%==TR (
			echo %RED%Error: No MSX turbo R default machine in OpenMSX%RESET%
			exit /b 1
		) else if /I %Version%==12 (
			set EmulatorArgs=!EmulatorArgs! -machine C-BIOS_MSX1
		)
	)
	if %Emul60Hz%==1 (
		echo %YELLOW%Warning: Emul60Hz can't be use with OpenMSX%RESET%
	)
	if %EmulSetFullScreen%==1 (
		echo %YELLOW%Warning: EmulSetFullScreen can't be use with OpenMSX%RESET%
	)
	if %EmulMute%==1 (
		set EmulatorArgs=!EmulatorArgs! -mute_channels
	)
	if %EmulAddExpander%==1 (
		set EmulatorArgs=!EmulatorArgs! -ext slotexpander
	)

	rem ---- Add launch program ----
	if /I %Ext%==bin ( set EmulatorArgs=!EmulatorArgs! -diska %ProjDir%\emul\dsk )
	if /I %Ext%==rom ( set EmulatorArgs=!EmulatorArgs! -cart %ProjDir%\emul\rom\%ProjName%.rom )
	if /I %Ext%==com ( set EmulatorArgs=!EmulatorArgs! -diska %ProjDir%\emul\dos -ext msxdos2 )
)
rem ***************************************************************************
rem * BlueMSX                                                                 *
rem ***************************************************************************
rem  Doc: http://www.msxblue.com/manual/commandlineargs_c.htm
if /I %EmulatorName%==bluemsx (

	echo Note: You can only change frequency by setting the machine

	rem ---- Add launch options ----
	if %EmulSetMachine%==1 (
		if /I %Version%==1 (
			if %Emul60Hz%==1 (
				set EmulatorArgs=!EmulatorArgs! /machine "MSX - Japanese"
			) else (
				set EmulatorArgs=!EmulatorArgs! /machine "MSX"
			)
		) else if /I %Version%==2 (
			if %Emul60Hz%==1 (
				set EmulatorArgs=!EmulatorArgs! /machine "MSX2 - Japanese"
			) else (
				set EmulatorArgs=!EmulatorArgs! /machine "MSX2"
			)
		) else if /I %Version%==2P (
			if %Emul60Hz%==1 (
				set EmulatorArgs=!EmulatorArgs! /machine "MSX2+"
			) else (
				set EmulatorArgs=!EmulatorArgs! /machine "MSX2+ - C-BIOS"
				echo Note: 50Hz MSX2+ is C-BIOS based with BlueMSX
			)
		) else if /I %Version%==TR (
			if %Emul60Hz%==1 (
				set EmulatorArgs=!EmulatorArgs! /machine "MSXturboR"
			) else (
				set EmulatorArgs=!EmulatorArgs! /machine "MSXturboR"
				echo %YELLOW%Warning: MSXturboR is only 60Hz with BlueMSX%RESET%
			)
		) else if /I %Version%==12 (
			if %Emul60Hz%==1 (
				set EmulatorArgs=!EmulatorArgs! /machine "MSX"
			) else (
				set EmulatorArgs=!EmulatorArgs! /machine "MSX - Japanese"
			)
		)
	)
	if %EmulSetFullScreen%==1 (
		set EmulatorArgs=!EmulatorArgs! /fullscreen
	)
	if %EmulMute%==1 (
		echo %YELLOW%Warning: EmulMute can't be use with BlueMSX%RESET%
	)
	if %EmulAddExpander%==1 (
		echo %YELLOW%Warning: EmulAddExpander can't be use with BlueMSX%RESET%
	)

	rem ---- Add launch program ----
	if /I %Ext%==bin ( set EmulatorArgs=!EmulatorArgs! /diskA %ProjDir%\emul\bin\dsk\%ProjName%.dsk )
	if /I %Ext%==rom ( set EmulatorArgs=!EmulatorArgs! /rom1 %ProjDir%\emul\rom\%ProjName%.rom )
	if /I %Ext%==com ( set EmulatorArgs=!EmulatorArgs! /diskA %ProjDir%\emul\dos\dsk\%ProjName%.dsk )
)
rem ***************************************************************************
rem * fMSX                                                                    *
rem ***************************************************************************
rem  Doc: https://fms.komkon.org/fMSX/fMSX.html#LABI
if /I %EmulatorName%==fmsx (
	echo Note: Command line parameters are only fonctionnal since fMSX 6.0

	rem ---- Add launch options ----
	if %EmulSetMachine%==1 (
		if /I %Version%==1 (
			set EmulatorArgs=!EmulatorArgs! -msx1
		) else if /I %Version%==2 (
			set EmulatorArgs=!EmulatorArgs! -msx2
		) else if /I %Version%==2P (
			set EmulatorArgs=!EmulatorArgs! -msx2+
		) else if /I %Version%==TR (
			echo %RED%Error: No MSX turbo R support in fMSX%RESET%
			exit /b 1
		) else if /I %Version%==12 (
			set EmulatorArgs=!EmulatorArgs! -msx1
		)
	)
	if %Emul60Hz%==1 (
		set EmulatorArgs=!EmulatorArgs! -ntsc
	) else (
		set EmulatorArgs=!EmulatorArgs! -pal
	)
	if %EmulSetFullScreen%==1 (
		echo %YELLOW%Warning: EmulSetFullScreen can't be use with fMSX%RESET%
	)
	if %EmulMute%==1 (
		set EmulatorArgs=!EmulatorArgs! -nosound
	)
	if %EmulAddExpander%==1 (
		echo %YELLOW%Warning: EmulAddExpander can't be use with fMSX%RESET%
	)

	rem ---- Add launch program ----
	if /I %Ext%==bin ( set EmulatorArgs=!EmulatorArgs! -diska %ProjDir%\emul\bin\dsk\%ProjName%.dsk )
	if /I %Ext%==rom ( set EmulatorArgs=!EmulatorArgs! %ProjDir%\emul\rom\%ProjName%.rom )
	if /I %Ext%==com ( set EmulatorArgs=!EmulatorArgs! -diska %ProjDir%\emul\dos\dsk\%ProjName%.dsk )
)
rem ***************************************************************************
rem * Mesei                                                                   *
rem ***************************************************************************
REM if /I %EmulatorName%==meisei (
REM )
rem ***************************************************************************
rem * Emulicious                                                              *
rem ***************************************************************************
if /I %EmulatorName%==emulicious (

	rem ---- Add launch options ----
	if %EmulSetMachine%==1 (
		echo %YELLOW%Warning: EmulSetMachine can't be use with Emulicious%RESET%
	)
	if %Emul60Hz%==1 (
		echo %YELLOW%Warning: Emul60Hz can't be use with Emulicious%RESET%
	)
	if %EmulSetFullScreen%==1 (
		echo %YELLOW%Warning: EmulSetFullScreen can't be use with Emulicious%RESET%
	)
	if %EmulMute%==1 (
		set EmulatorArgs=!EmulatorArgs! -muted
	)
	if %EmulAddExpander%==1 (
		echo %YELLOW%Warning: EmulAddExpander can't be use with Emulicious%RESET%
	)

	rem ---- Add launch program ----
	if /I %Ext%==rom ( 
		set EmulatorArgs=!EmulatorArgs! %ProjDir%\emul\rom\%ProjName%.rom 
	) else (
		echo %RED%Error: Emulicious only support ROM format%RESET%
		exit /b 1
	)
	
	REM -muted = start without sound (can still be manually enabled, see controls below)
	REM -scale [level] = start with given zoom level
	REM -link [address] = connects your Emulicious via link with the given address (e.g. "-link localhost" to connect to yourself) 
	REM -throttle [speed] = throttles the speed to the given value in percent
	REM -disassemble [file] = Disassemble the provided file. If a directory is provided, all contained files are disassembled.
)
rem ***************************************************************************
rem * RuMSX                                                                   *
rem ***************************************************************************
if /I %EmulatorName%==msxw (
	set EmulatorName=msx
)

if /I %EmulatorName%==msx (
	rem ---- Add launch options ----
	if %EmulSetMachine%==1 (
		echo %YELLOW%Warning: EmulSetMachine can't be use with RuMSX%RESET%
	)
	if %Emul60Hz%==1 (
		echo %YELLOW%Warning: Emul60Hz can't be use with RuMSX%RESET%
	)
	if %EmulSetFullScreen%==1 (
		echo %YELLOW%Warning: EmulSetFullScreen can't be use with RuMSX%RESET%
	)
	if %EmulMute%==1 (
		set EmulatorArgs=!EmulatorArgs! -NoSoundOut
	)
	if %EmulAddExpander%==1 (
		echo %YELLOW%Warning: EmulAddExpander can't be use with RuMSX%RESET%
	)

	rem ---- Add launch program ----
	if /I %Ext%==bin ( set EmulatorArgs=!EmulatorArgs! -dirAsDisk -disk %ProjDir%\emul\bin )
	if /I %Ext%==rom ( set EmulatorArgs=!EmulatorArgs! -rom %ProjDir%\emul\rom\%ProjName%.rom )
	if /I %Ext%==com ( set EmulatorArgs=!EmulatorArgs! -dirAsDisk -disk %ProjDir%\emul\dos )
)
rem ***************************************************************************
rem * START EMULATOR                                                          *
rem ***************************************************************************
echo %EmulatorName% %EmulatorArgs%
start /b %Emulator% %EmulatorArgs%

rem ***************************************************************************
rem * START DEBUGGER                                                          *
rem ***************************************************************************
if %EmulStartDebugger%==1 (
	start /b %Debugger%
)
