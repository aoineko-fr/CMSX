//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "core.h"


//
void VDP_SetModeText1();
void VDP_SetModeMultiColor();
void VDP_SetModeGraphic1();
void VDP_SetModeGraphic2();

#if (MSX_VERSION >= MSX_2)

void VDP_SetModeText2();
void VDP_SetModeGraphic3();
void VDP_SetModeGraphic4();
void VDP_SetModeGraphic5();
void VDP_SetModeGraphic6();
void VDP_SetModeGraphic7();

#endif

//-----------------------------------------------------------------------------
//
inline void VDP_SetScreen(u8 mode);