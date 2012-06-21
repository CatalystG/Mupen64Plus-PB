#ifndef __GSPNEON_H__
#define __GSPNEON_H__

#include "Types.h"
#include "gDP.h"
#include "OpenGL.h"

void gSPTransformVertex4NEON(u32 v, float mtx[4][4]);
void gSPTransformNormal4NEON(u32 v, float mtx[4][4]);
void gSPLightVertex4NEON(u32 v);
void gSPBillboardVertex4NEON(u32 v);
void gSPTransformVertexNEON(float vtx[4], float mtx[4][4]);
void gSPLightVertexNEON(u32 v);

#endif
