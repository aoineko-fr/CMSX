..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply1_blk1.data.h -name g_Ply1_Blk1 -pos 0 24  -size 8 8 -num 26 3 -trans 0xFF00FF -bpc 1
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply1_blk2.data.h -name g_Ply1_Blk2 -pos 0 48  -size 8 8 -num 26 3 -trans 0xFF00FF -bpc 1
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply1_clth.data.h -name g_Ply1_Clth -pos 0 80  -size 8 8 -num 26 2 -trans 0xFF00FF -bpc 1
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply1_skin.data.h -name g_Ply1_Skin -pos 0 96  -size 8 8 -num 26 3 -trans 0xFF00FF -bpc 1
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply1_whit.data.h -name g_Ply1_Whit -pos 0 120 -size 8 8 -num 26 3 -trans 0xFF00FF -bpc 1
                                                                     
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply2_blk1.data.h -name g_Ply2_Blk1 -pos 0 168 -size 8 8 -num 26 3 -trans 0xFF00FF -bpc 1
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply2_blk2.data.h -name g_Ply2_Blk2 -pos 0 192 -size 8 8 -num 26 3 -trans 0xFF00FF -bpc 1
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply2_clth.data.h -name g_Ply2_Clth -pos 0 224 -size 8 8 -num 26 2 -trans 0xFF00FF -bpc 1
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply2_skin.data.h -name g_Ply2_Skin -pos 0 240 -size 8 8 -num 26 3 -trans 0xFF00FF -bpc 1
..\..\..\tools\CMSXimg\CMSXimg.exe players.png -out ply2_whit.data.h -name g_Ply2_Whit -pos 0 264 -size 8 8 -num 26 3 -trans 0xFF00FF -bpc 1

..\..\..\tools\CMSXimg\CMSXimg.exe court.png   -out court.data.h     -name g_Court     -mode gm2 
pause


REM --- Up part ---
-l i16 0 0 1 1 0x010101
-l i16 0 0 1 1 0x010101 0x5955E0 0x3AA241 0xCCCCCC 0xDB6559
-l i16 0 8 1 1 0x8076F1 0x3EB849 0x5955E0 0x3AA241
-l i16 0 0 1 1 0xFFFFFF 0xCCCCCC
-l i16 0 0 1 1 0xFF897D 0xDB6559

REM --- Bottom part ---
-l i16 0 16 1 1 0x010101
-l i16 8 16 1 1 0x010101
-l i16 0 16 1 1 0x010101 0x5955E0 0x3AA241 0xCCCCCC 0xDB6559
-l i16 8 16 1 1 0x010101 0x5955E0 0x3AA241 0xCCCCCC 0xDB6559
-l i16 0 16 1 1 0xFFFFFF 0xCCCCCC
-l i16 8 16 1 1 0xFFFFFF 0xCCCCCC
-l i16 0 16 1 1 0xFF897D 0xDB6559
-l i16 8 16 1 1 0xFF897D 0xDB6559
