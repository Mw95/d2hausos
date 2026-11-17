#ifndef D2RE_D2GLIDE_GLIDE3X_H
#define D2RE_D2GLIDE_GLIDE3X_H

#include <windows.h>

extern "C"
{
DWORD __stdcall grAlphaBlendFunction(DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall grAlphaCombine(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall grBufferClear(DWORD, DWORD, DWORD);
DWORD __stdcall grBufferSwap(DWORD);
DWORD __stdcall grChromakeyMode(DWORD);
DWORD __stdcall grChromakeyValue(DWORD);
DWORD __stdcall grColorCombine(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall grColorMask(DWORD, DWORD);
DWORD __stdcall grConstantColorValue(DWORD);
DWORD __stdcall grCoordinateSpace(DWORD);
DWORD __stdcall grDepthMask(DWORD);
DWORD __stdcall grDitherMode(DWORD);
DWORD __stdcall grDrawLine(const void *, const void *);
DWORD __stdcall grDrawPoint(const void *);
DWORD __stdcall grDrawVertexArray(DWORD, DWORD, void *);
DWORD __stdcall grDrawVertexArrayContiguous(DWORD, DWORD, void *, DWORD);
DWORD __stdcall grFinish(void);
DWORD __stdcall grGet(DWORD, DWORD, void *);
DWORD __stdcall grGetString(DWORD);
DWORD __stdcall grGlideInit(void);
DWORD __stdcall grGlideShutdown(void);
DWORD __stdcall grLfbLock(DWORD, DWORD, DWORD, DWORD, DWORD, void *);
DWORD __stdcall grLfbUnlock(DWORD, DWORD);
DWORD __stdcall grSstSelect(DWORD);
DWORD __stdcall grSstWinClose(DWORD);
DWORD __stdcall grSstWinOpen(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall grTexCombine(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD __stdcall grTexDownloadMipMap(DWORD, DWORD, DWORD, void *);
DWORD __stdcall grTexDownloadTable(DWORD, void *);
DWORD __stdcall grTexFilterMode(DWORD, DWORD, DWORD);
DWORD __stdcall grTexMaxAddress(DWORD);
DWORD __stdcall grTexMinAddress(DWORD);
DWORD __stdcall grTexSource(DWORD, DWORD, DWORD, void *);
DWORD __stdcall grVertexLayout(DWORD, DWORD, DWORD);
DWORD __stdcall guGammaCorrectionRGB(float, float, float);
}

#endif
