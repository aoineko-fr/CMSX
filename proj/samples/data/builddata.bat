..\..\..\tools\MSXImage\MSXImage.exe -in data.png -out data_bmp_8b.h -pos 16 32 -size 16 16 -num 1 1 -name g_DataBmp8b -trans 0x8468a1 -bpc 8
..\..\..\tools\MSXImage\MSXImage.exe -in data.png -out data_bmp_4b.h -pos 16 32 -size 16 16 -num 1 1 -name g_DataBmp4b -trans 0x8468a1 -bpc 4 -pal msx1
..\..\..\tools\MSXImage\MSXImage.exe -in data.png -out data_bmp_2b.h -pos 16 32 -size 16 16 -num 1 1 -name g_DataBmp2b -trans 0x8468a1 -bpc 2 -pal custom
..\..\..\tools\MSXImage\MSXImage.exe -in data.png -out data_sprt_8.h -pos 0 144 -size 8 8 -num 32 3 -name g_DataSprt8 -trans 0x8468a1 -bpc 1
..\..\..\tools\MSXImage\MSXImage.exe -in data.png -out data_sprt_16.h -pos 0 80 -size 8 8 -num 12 4 -name g_DataSprt16 -trans 0x8468a1 -bpc 1
..\..\..\tools\MSXImage\MSXImage.exe -in data.png -out data_sprt_16or.h -pos 128 80 -size 8 8 -num 12 4 -name g_DataSprt16or -trans 0x8468a1 -bpc 1
..\..\..\tools\MSXImage\MSXImage.exe -in data.png -out data_sprt_16il.h -pos 0 160 -size 8 8 -num 12 8 -name g_DataSprt16il -trans 0x8468a1 -bpc 1
pause