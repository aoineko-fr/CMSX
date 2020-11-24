//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "core.h"
#include "3d.h"
#include "trigo64.inc"
#include "proj64.inc"

// must be const!
ShortVec g_Camera;
ShortVec g_Position;
u8 g_Angle, g_AngleIndex;

#define POINT_NUM 8//5
ShortVec g_Local[POINT_NUM];
ShortVec g_World[POINT_NUM];
ShortVec g_Screen[POINT_NUM];

/**
 *
 */
void SetShortVec(ShortVec* ret, i16 x, i16 y, i16 z)
{
	ret->x = x;
	ret->y = y;
	ret->z = z;
}

/**
 *
 */
void TransXZ(ShortVec* ret, const ShortVec* vec, u8 g_Angle, const ShortVec* pos)
{
	g_Angle >>= 2; 
	ret->x = UxU(vec->x, g_Cosinus[g_Angle]) - UxU(vec->z, g_Sinus[g_Angle]);
	ret->y = vec->y + pos->y;
	ret->z = UxU(vec->x, g_Sinus[g_Angle]) + UxU(vec->z, g_Cosinus[g_Angle]);
}

/**
 * 
 */
void TransXZIndex(i8 i)
{
	g_World[i].x = UxU(g_Local[i].x, g_Cosinus[g_AngleIndex]) - UxU(g_Local[i].z, g_Sinus[g_AngleIndex]) + g_Position.x;
	g_World[i].y = g_Local[i].y + g_Position.y;
	g_World[i].z = UxU(g_Local[i].x, g_Sinus[g_AngleIndex]) + UxU(g_Local[i].z, g_Cosinus[g_AngleIndex]) + g_Position.z;
}

/**
 *
 */
void Project(ShortVec* ret, const ShortVec* vec, const ShortVec* cam)
{
#if 1
	ret->z = ((vec->z - cam->z) >> 5);
	ret->x = 128 + (vec->x - cam->x) / ret->z;
	ret->y = 106 + (vec->y - cam->y) / ret->z;
#else
	ret->z = ((vec->z - cam->z) >> 5);
	if(ret->z > 0)
	{
		ret->x = 0;
		ret->y = 0;
	}
	else
	{
		ret->x = 128 + UxU2(vec->x - cam->x, g_ProjectionX[ret->z]);
		ret->y = 106 + UxU2(vec->y - cam->y, g_ProjectionY[ret->z]);
	}
#endif
}
