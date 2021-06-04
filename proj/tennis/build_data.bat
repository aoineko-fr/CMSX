cd .\datasrc

@echo =============================================================================
@echo  ROM DATA - PAGE 0
@echo =============================================================================

@echo.
@echo -----------------------------------------------------------------------------
@echo  Convert Players Sprites...
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ..\content\data_player1.h -mode sprt -name g_DataPlayer1 -pos 0 0 -size 16 24 -num 9 3 -at 0x0200 ^
	-l i16 0  0 1 1 0x010101 ^
	-l i16 0  0 1 1 0x010101 0x403B78 0x7F7F7F 0x7F453F ^
	-l i16 0  8 1 1 0x8076F1 0x403B78 ^
	-l i16 0  0 1 1 0xFFFFFF 0x7F7F7F ^
	-l i16 0  0 1 1 0xFF897D 0x7F453F ^
	-l i8  0 16 2 1 0x010101 ^
	-l i8  0 16 2 1 0x010101 0x403B78 0x7F7F7F 0x7F453F ^
	-l i8  0 16 2 1 0xFFFFFF 0x7F7F7F ^
	-l i8  0 16 2 1 0xFF897D 0x7F453F

..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ..\content\data_player2.h -mode sprt -name g_DataPlayer2 -pos 0 96 -size 16 24 -num 9 3 -at %errorlevel% ^
	-l i8  0 0 2 1 0x010101 ^
	-l i8  0 0 2 1 0x010101 0x1F5C24 0x7F7F7F 0x7F453F ^
	-l i8  0 0 2 1 0xFFFFFF 0x7F7F7F ^
	-l i8  0 0 2 1 0xFF897D 0x7F453F ^
	-l i16 0 8 1 1 0x010101 ^
	-l i16 0 8 1 1 0x010101 0x1F5C24 0x7F7F7F 0x7F453F ^
	-l i16 0 0 1 1 0x3EB849 0x1F5C24 ^
	-l i16 0 8 1 1 0xFFFFFF 0x7F7F7F ^
	-l i16 0 8 1 1 0xFF897D 0x7F453F
	
..\..\..\tools\CMSXimg\CMSXimg.exe logo_ball.png -out ..\content\data_logo_ball.h -mode sprt -name g_DataLogoBall -pos 164 41 -size 16 16 -num 2 2  -at %errorlevel% ^
	-l i16 0 0 1 1 0xDED087 ^
	-l i16 0 0 1 1 0xB95E51

@echo.
@echo -----------------------------------------------------------------------------
@echo  Convert SFX...

..\..\..\tools\CMSXbin\CMSXbin.exe ayfx_bank.afb -o ..\content\data_sfx.h -t g_DataSFX -ad -at %errorlevel%

@echo.
@echo -----------------------------------------------------------------------------
@echo  Convert Music...

..\..\..\tools\CMSXbin\CMSXbin.exe intro.pt3 -o ..\content\data_music.h -t g_DataMusic -skip 0 100 -ad -pt3 -at %errorlevel%

@echo.
@echo Page 0 Last Address: %errorlevel%
	
@echo.
@echo =============================================================================
@echo  ROM DATA - PAGE 1-2
@echo =============================================================================

@echo -----------------------------------------------------------------------------
@echo  Convert SC2 Images...

..\..\..\tools\CMSXimg\CMSXimg.exe logo.png  -out ..\content\data_logo.h    -mode gm2   -compress rlep	-name g_DataLogo    -pos 32 16 -size 152 80  -offset 0
@echo %errorlevel%
                                                                                                            
..\..\..\tools\CMSXimg\CMSXimg.exe court.png -out ..\content\data_court.h   -mode gm2   -compress rlep	-name g_DataCourt   -pos 24 24 -size 216 144 -offset 0
@echo %errorlevel%

..\..\..\tools\CMSXimg\CMSXimg.exe misc.png  -out ..\content\data_referee.h -mode gm2   -compress rlep	-name g_DataReferee -pos 0 0   -size 96  48  -offset 208
@echo %errorlevel%

..\..\..\tools\CMSXimg\CMSXimg.exe score.png -out ..\content\data_board.h   -mode gm2   -compress rlep	-name g_DataScore   -pos 0 0   -size 216 80  -offset 160 ^
	-l gm2 0 88 112 96 ^
	-l gm2 112 80 72 24 ^
	-l gm2 184 80 72 24 ^
	-l gm2 112 104 72 16 ^
	-l gm2 184 104 72 16
@echo %errorlevel%

@echo.
@echo -----------------------------------------------------------------------------
@echo  Convert Fonts...

..\..\..\tools\CMSXimg\CMSXimg.exe font.png -out ..\content\data_font.h    -pos 0 0   -size 8 8 -gap 0 0 -num 16 4 -trans 0x000000 -bpc 1 -name g_DataFont    -font 8 8 ! _ -skip

..\..\..\tools\CMSXimg\CMSXimg.exe misc.png -out ..\content\data_scrfont.h -pos 0 136 -size 8 8 -gap 0 0 -num 15 1 -trans 0x000000 -bpc 1 -name g_DataSrcFont -font 8 8 0 0x3E

@echo.
@echo -----------------------------------------------------------------------------
@echo  Convert Sprites...

..\..\..\tools\CMSXimg\CMSXimg.exe misc.png    -out ..\content\data_net.h     -mode sprt -name g_DataNet     -pos 0 200 -size 16 16 -num 5 1 -l i16 0 0 1 1 0xFFFFFF 0xCCCCCC

..\..\..\tools\CMSXimg\CMSXimg.exe misc.png    -out ..\content\data_points.h  -mode sprt -name g_DataPoints  -pos 0 232 -size 16 16 -num 8 1 -l i16 0 0 1 1 0xFFFFFF

..\..\..\tools\CMSXimg\CMSXimg.exe misc.png    -out ..\content\data_event.h   -mode sprt -name g_DataEvent   -pos 0 248 -size 16 16 -num 6 3 -l i16 0 0 1 1 0xFFFFFF

..\..\..\tools\CMSXimg\CMSXimg.exe misc.png    -out ..\content\data_racket.h  -mode bmp  -name g_DataRacket  -pos 128 200 -size 8 8 -num 3 1  -bpc 1 -trans 0xFF897D

..\..\..\tools\CMSXimg\CMSXimg.exe misc.png    -out ..\content\data_ball.h    -mode bmp  -name g_DataBall    -pos 128 192 -size 8 8 -num 10 1 -bpc 1 -trans 0xFF897D

..\..\..\tools\CMSXimg\CMSXimg.exe misc.png    -out ..\content\data_cup.h     -mode bmp  -name g_DataCup     -pos 128 208 -size 8 8 -num 6 1  -bpc 1 -trans 0xFF897D

..\..\..\tools\CMSXimg\CMSXimg.exe misc.png    -out ..\content\data_cursor.h  -mode sprt -name g_DataCursor  -pos 128 232 -size 16 16 -num 3 1 -l i16 0 0 1 1 0x5955E0

..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ..\content\data_launcher0.h -mode sprt -name g_DataLauncher0 -pos 160 88 -size 16 40 -num 1 1 ^
	-l i16 0  0 1 2 0x010101 ^
	-l i16 0  0 1 2 0x010101 0x706944 0x1F5C24 0x7F7F7F ^
	-l i16 0  0 1 1 0xDED087 0x706944 ^
	-l i16 0  8 1 1 0x3EB849 0x1F5C24 ^
	-l i16 0 16 1 1 0xFFFFFF 0x7F7F7F

..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ..\content\data_launcher1.h -mode sprt -name g_DataLauncher1 -pos 176 88 -size 16 40 -num 1 1 ^
	-l i16 0  8 1 2 0x010101 ^
	-l i16 0  8 1 2 0x010101 0x706944 0x1F5C24 0x7F7F7F ^
	-l i16 0  7 1 1 0xDED087 0x706944 ^
	-l i16 0 11 1 1 0x3EB849 0x1F5C24 ^
	-l i16 0 23 1 1 0xFFFFFF 0x7F7F7F


pause
cd ..