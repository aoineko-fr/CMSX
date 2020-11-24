//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

//----------------------------------------
// M A C R O S

#define M2U(a)     ((a) << 8)
#define U2M(a)     ((a) >> 8)
#define UxU(a, b)  (((a) >> 4) * ((b) >> 4))
#define UxU2(a, b) ((((a) >> 4) * (b)) >> 4)

//----------------------------------------
// T Y P E S

//typedef Vec3 int[3];
//typedef struct { i16 x, y, z, w; } ShortVec;
typedef struct { i16 x, y, z; } ShortVec;
//typedef ShortMat3 ShortVec[3];
//typedef ShortMat4 ShortVec[4];

//----------------------------------------
// P R  O T Y P E S

void SetShortVec(ShortVec* ret, i16 x, i16 y, i16 z);
void TransXZ(ShortVec* ret, const ShortVec* vec, u8 g_Angle, const ShortVec* pos);
void Project(ShortVec* ret, const ShortVec* vec, const ShortVec* cam);
void TransXZIndex(i8 i);
