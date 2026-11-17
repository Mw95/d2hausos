#ifndef D2RE_D2DIRECT3D_D3DCORE_H
#define D2RE_D2DIRECT3D_D3DCORE_H

#include <windows.h>
#include <stdarg.h>

struct D3DDRAWCTX
{
 	DWORD		dwPerspective;
 	DWORD		dwPerspectiveAllowed;
 	DWORD		dwFlatLighting;
 	DWORD		unobserved_00c;
 	DWORD		dwVSync;
 	DWORD		dwBlendShadow;
};

struct D3DBLITTABLE
{
 	void		*unobserved_000[7];
};

struct D3DENVINFO
{
 	DWORD		field_000[69];
 	const BYTE	*pbPalette;
 	const BYTE	*pbColorMap;
 	DWORD		field_11c;
};

struct D3DSURFACE;
struct D3DTEXTURE;

struct D3DSURFDESC
{
 	DWORD		dwSize;
 	DWORD		dwFlags;
 	DWORD		dwHeight;
 	DWORD		dwWidth;
 	int			nPitch;
 	DWORD		field_14;
 	DWORD		field_18[3];
 	BYTE		*pbSurface;
 	DWORD		field_28[8];
 	DWORD		adwPixelFormat[8];
 	DWORD		field_68;
 	DWORD		field_6c;
 	DWORD		field_70[3];
};

struct D3DPIXELFORMAT
{
 	DWORD		field_00[8];
};

struct D3DCAPSBLOCK
{
 	DWORD		field_00;
 	DWORD		field_04[3];
};

struct D3DVIEWDATA
{
 	DWORD		dwSize;
 	DWORD		field_04;
 	DWORD		field_08;
 	DWORD		dwWidth;
 	DWORD		dwHeight;
 	float		field_14;
 	float		field_18;
 	float		flWidth;
 	float		flHeight;
 	float		field_24;
 	float		field_28;
};

struct D3DTLVERTEX
{
 	float		flX;
 	float		flY;
 	float		field_08;
 	float		field_0c;
 	DWORD		dwColor;
 	DWORD		field_14;
 	float		flU;
 	float		flV;
};

struct D3DVBDESC
{
 	DWORD		dwSize;
 	DWORD		field_04;
 	DWORD		field_08;
 	DWORD		dwNumVertices;
};

struct D3DVERTEXBUFFER;
struct D3DVERTEXBUFFERVTBL
{
 	void		*unobserved_000[2];
 	DWORD		(__stdcall *field_08)(D3DVERTEXBUFFER *ptThis);
 	HRESULT		(__stdcall *field_0c)(D3DVERTEXBUFFER *ptThis, DWORD dwFlags,
					D3DTLVERTEX **ppVertices,
					DWORD *pdwSize);
 	HRESULT		(__stdcall *field_10)(D3DVERTEXBUFFER *ptThis);
};
struct D3DVERTEXBUFFER
{
 	D3DVERTEXBUFFERVTBL *lpVtbl;
};

struct D3DGAMMARAMP
{
 	WORD		field_000[256];
 	WORD		field_200[256];
 	WORD		field_400[256];
};

struct D3DDDOBJECT;
struct D3DDDOBJECTVTBL
{
 	HRESULT		(__stdcall *field_00)(D3DDDOBJECT *ptThis, const GUID *pIid, void **ppOut);
 	void		*unobserved_004;
 	DWORD		(__stdcall *field_08)(D3DDDOBJECT *ptThis);
 	void		*unobserved_00c[2];
 	HRESULT		(__stdcall *field_14)(D3DDDOBJECT *ptThis, DWORD dwFlags, PALETTEENTRY *ptEntries,
					void **pptPalette, void *pUnused);
 	HRESULT		(__stdcall *pfnCreateSurface)(D3DDDOBJECT *ptThis, D3DSURFDESC *ptDesc,
					D3DSURFACE **pptSurface, void *pUnused);
 	void		*unobserved_01c[13];
 	HRESULT		(__stdcall *pfnSetCooperativeLevel)(D3DDDOBJECT *ptThis, HWND hWnd,
					DWORD dwFlags);
 	HRESULT		(__stdcall *pfnSetDisplayMode)(D3DDDOBJECT *ptThis, DWORD dwWidth, DWORD dwHeight,
					DWORD dwBits, DWORD dwRate, DWORD dwFlags);
 	void		*unobserved_058;
 	HRESULT		(__stdcall *field_5c)(D3DDDOBJECT *ptThis, D3DCAPSBLOCK *ptCaps, DWORD *pdwTotal,
					DWORD *pdwFree);
 	void		*unobserved_060[3];
 	HRESULT		(__stdcall *field_6c)(D3DDDOBJECT *ptThis, BYTE *pbIdentifier, DWORD dwFlags);
};
struct D3DDDOBJECT
{
 	D3DDDOBJECTVTBL *lpVtbl;
};

struct D3DVIEWPORT;
struct D3DDEVICE;
struct D3DOBJECT;
struct D3DOBJECTVTBL
{
 	void		*unobserved_000[2];
 	DWORD		(__stdcall *field_08)(D3DOBJECT *ptThis);
 	void		*unobserved_00c[3];
 	HRESULT		(__stdcall *field_18)(D3DOBJECT *ptThis, D3DVIEWPORT **pptViewport,
					void *pUnused);
 	void		*unobserved_01c;
 	HRESULT		(__stdcall *field_20)(D3DOBJECT *ptThis, const GUID *pIid, D3DSURFACE *ptTarget,
					D3DDEVICE **pptDevice, void *pUnused);
 	HRESULT		(__stdcall *field_24)(D3DOBJECT *ptThis, D3DVBDESC *ptDesc,
					D3DVERTEXBUFFER **pptBuffer,
					DWORD dwFlags, void *pUnused);
};
struct D3DOBJECT
{
 	D3DOBJECTVTBL *lpVtbl;
};

struct D3DVIEWPORTVTBL
{
 	void		*unobserved_000[2];
 	DWORD		(__stdcall *field_08)(D3DVIEWPORT *ptThis);
 	void		*unobserved_00c[14];
 	HRESULT		(__stdcall *pfnSetViewport)(D3DVIEWPORT *ptThis, D3DVIEWDATA *ptData);
 	void		*unobserved_048[2];
 	HRESULT		(__stdcall *field_50)(D3DVIEWPORT *ptThis, DWORD dwCount, RECT *prc,
					DWORD dwFlags,
					DWORD dwColor, DWORD dwZ, DWORD dwStencil);
};
struct D3DVIEWPORT
{
 	D3DVIEWPORTVTBL *lpVtbl;
};

struct D3DDEVICEVTBL
{
 	void		*unobserved_000[2];
 	DWORD		(__stdcall *field_08)(D3DDEVICE *ptThis);
 	void		*unobserved_00c[2];
 	HRESULT		(__stdcall *field_14)(D3DDEVICE *ptThis, D3DVIEWPORT *ptViewport);
 	void		*unobserved_018[3];
 	HRESULT		(__stdcall *pfnBeginScene)(D3DDEVICE *ptThis);
 	HRESULT		(__stdcall *pfnEndScene)(D3DDEVICE *ptThis);
 	void		*unobserved_02c;
 	HRESULT		(__stdcall *pfnSetCurrentViewport)(D3DDEVICE *ptThis, D3DVIEWPORT *ptViewport);
 	void		*unobserved_034[9];
 	HRESULT		(__stdcall *field_58)(D3DDEVICE *ptThis, DWORD dwState, DWORD dwValue);
 	void		*unobserved_05c[5];
 	HRESULT		(__stdcall *field_70)(D3DDEVICE *ptThis, DWORD dwType, DWORD dwFormat,
					D3DTLVERTEX *ptVertices,
					DWORD dwCount, DWORD dwFlags);
 	HRESULT		(__stdcall *field_74)(D3DDEVICE *ptThis, DWORD dwType, DWORD dwFormat,
					D3DTLVERTEX *ptVertices,
					DWORD dwCount, const WORD *pwIndices, DWORD dwIndexCount, DWORD dwFlags);
 	void		*unobserved_078[5];
 	HRESULT		(__stdcall *field_8c)(D3DDEVICE *ptThis, DWORD dwType, D3DVERTEXBUFFER *ptBuffer,
					const WORD *pwIndices, DWORD dwIndexCount, DWORD dwFlags);
 	void		*unobserved_090[2];
 	HRESULT		(__stdcall *field_98)(D3DDEVICE *ptThis, DWORD dwStage, D3DTEXTURE *ptTexture);
 	void		*unobserved_09c;
 	HRESULT		(__stdcall *field_a0)(D3DDEVICE *ptThis, DWORD dwStage, DWORD dwState,
					DWORD dwValue);
};
struct D3DDEVICE
{
 	D3DDEVICEVTBL *lpVtbl;
};

struct D3DSURFACEVTBL
{
 	HRESULT		(__stdcall *field_00)(D3DSURFACE *ptThis, const GUID *pIid, void **ppOut);
 	void		*unobserved_004;
 	DWORD		(__stdcall *field_08)(D3DSURFACE *ptThis);
 	void		*unobserved_00c[8];
 	HRESULT		(__stdcall *field_2c)(D3DSURFACE *ptThis, D3DSURFACE *ptTarget, DWORD dwFlags);
 	HRESULT		(__stdcall *pfnGetAttachedSurface)(D3DSURFACE *ptThis, D3DCAPSBLOCK *ptCaps,
					D3DSURFACE **pptSurface);
 	void		*unobserved_034[12];
 	HRESULT		(__stdcall *pfnLock)(D3DSURFACE *ptThis, RECT *prc, D3DSURFDESC *ptDesc,
					DWORD dwFlags,
					HANDLE hEvent);
 	void		*unobserved_068[5];
 	HRESULT		(__stdcall *pfnSetPalette)(D3DSURFACE *ptThis, void *ptPalette);
 	HRESULT		(__stdcall *pfnUnlock)(D3DSURFACE *ptThis, void *pRect);
};
struct D3DSURFACE
{
 	D3DSURFACEVTBL *lpVtbl;
};

struct D3DTEXTUREVTBL
{
 	void		*unobserved_000[2];
 	DWORD		(__stdcall *field_08)(D3DTEXTURE *ptThis);
 	void		*unobserved_00c[2];
 	HRESULT		(__stdcall *field_14)(D3DTEXTURE *ptThis, D3DTEXTURE *ptSource);
};
struct D3DTEXTURE
{
 	D3DTEXTUREVTBL *lpVtbl;
};

struct D3DGAMMA;
struct D3DGAMMAVTBL
{
 	void		*unobserved_000[2];
 	DWORD		(__stdcall *field_08)(D3DGAMMA *ptThis);
 	HRESULT		(__stdcall *field_0c)(D3DGAMMA *ptThis, DWORD dwFlags, D3DGAMMARAMP *ptRamp);
 	HRESULT		(__stdcall *field_10)(D3DGAMMA *ptThis, DWORD dwFlags, D3DGAMMARAMP *ptRamp);
};
struct D3DGAMMA
{
 	D3DGAMMAVTBL *lpVtbl;
};

extern "C" HRESULT __stdcall DirectDrawCreate(void *pGuid, D3DDDOBJECT **pptObject, void *pUnused);

#define D3D_FLOAT_STORE(f)	((void)0)

extern D3DDRAWCTX *gptDrawCtx;
extern D3DBLITTABLE *gptBlitTable;
extern D3DENVINFO *gptEnvInfo;
struct D3DVIDEOCONFIG
{ DWORD dwFlags; BYTE abGuid[16]; char szDeviceName[256]; };
extern D3DVIDEOCONFIG gtVideoConfig;
extern int gnScreenWidth;
extern int gnScreenHeight;
extern D3DDDOBJECT *glpDD;
extern D3DSURFACE *glpddsPrimary;
extern D3DSURFACE *glpddsBack;
extern D3DDEVICE *glpDevice;
extern D3DOBJECT *glpD3D;
extern D3DVIEWPORT *glpViewport;
extern DWORD gfSuspended;
extern CRITICAL_SECTION *pcsPause;
extern HWND ghWnd;
extern int gnTexelBytes;
extern BOOL gfUseAgp;
extern DWORD gadwPalette[256];
extern WORD gawPalette[256];
extern int gnFloorCells;
extern int gnFloorRows;
extern BYTE gabLightTable[256][256];

void __fastcall Direct3DPlayBinkFile(const char *pszFileName, int nMode,
	void (__stdcall *pfnTick)(void));

void __fastcall D3DLogDDError(const char *pszMessage, HRESULT hResult);
void __fastcall D3DLogError(const char *pszMessage, HRESULT hResult);
void __fastcall D3DSetBlendMode(int nMode);
void __fastcall D3DSetAlphaMode(int nMode);
void __fastcall D3DSetFilterMode(int nMode);
BOOL __fastcall D3DCreateMemoryTexture(D3DSURFACE **pptSurface, D3DTEXTURE **pptTexture, int nWidth,
	int nHeight);
BOOL __fastcall D3DCreateDeviceTexture(D3DSURFACE **pptSurface, D3DTEXTURE **pptTexture, int nWidth,
	int nHeight);
void __fastcall D3DCopySurface(D3DSURFACE *ptDest, D3DSURFACE *ptSource, int nRows);
void __fastcall D3DCopyRows(const BYTE *pbSource, BYTE *pbDest, int nWidth, int nRows, int nDestPitch,
	DWORD dwUnused);
void __fastcall D3DExpandToWords(const BYTE *pbSource, WORD *pwDest, int nWidth, int nRows, int nDestPitch,
	DWORD dwUnused);
void __fastcall D3DExpandToDwords(const BYTE *pbSource, DWORD *pdwDest, int nWidth, int nRows,
	int nDestPitch, DWORD dwUnused);
DWORD __fastcall D3DGetPaletteColor(DWORD bIndex);
void WINAPI D3DClearRender(void);

#endif
