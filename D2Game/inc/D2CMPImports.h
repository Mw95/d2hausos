#ifndef D2RE_D2CMP_IMPORTS_WRAPPERS_H
#define D2RE_D2CMP_IMPORTS_WRAPPERS_H

#include "D2CMP.imports.h"

#define D2CMPPaletteCreateTables(pPal) \
	((BYTE *)D2CMPOrdinal10000((DWORD)(pPal)))

#define D2CMPPaletteCreateBlendTable(dwPalette, pdwSize) \
	D2CMPOrdinal10001((DWORD)(dwPalette), (DWORD)(pdwSize))

#define D2CMPPaletteFreeTable(pTable) \
	D2CMPOrdinal10002((DWORD)(pTable))

#define D2CMPPaletteCreateAddTable(pbPalette, dwUnused) \
	D2CMPOrdinal10003((DWORD)(pbPalette), (DWORD)(dwUnused))

#define D2CMPPaletteFindNearest(pbPalette, dwEntries, nColor0, nColor1, nColor2) \
	D2CMPOrdinal10004((DWORD)(pbPalette), (DWORD)(dwEntries), (DWORD)(nColor0), (DWORD)(nColor1), \
		(DWORD)(nColor2))

#define D2CMPPaletteFindFarthest(pbPalette, dwEntries, nColor0, nColor1, nColor2) \
	D2CMPOrdinal10005((DWORD)(pbPalette), (DWORD)(dwEntries), (DWORD)(nColor0), (DWORD)(nColor1), \
		(DWORD)(nColor2))

#define D2CMPPaletteLoad(pszPalette, PaletteStartEntry, PaletteEndEntry) \
	((BOOL)D2CMPOrdinal10006((DWORD)(pszPalette), (DWORD)(PaletteStartEntry), (DWORD)(PaletteEndEntry)))

#define D2CMPPaletteMatchColor(bColor2, bColor1, bColor0) \
	D2CMPOrdinal10007((DWORD)(bColor2), (DWORD)(bColor1), (DWORD)(bColor0))

#define D2CMPPaletteQuantize(pbRgb, pbOut, dwCount, a4) \
	D2CMPOrdinal10008((DWORD)(pbRgb), (DWORD)(pbOut), (DWORD)(dwCount), (DWORD)(a4))

#define D2CMPPaletteSetKeyColor(dwColor) \
	D2CMPOrdinal10009((DWORD)(dwColor))

#define D2CMPPaletteSetMatchKeyColor(fMatch) \
	D2CMPOrdinal10010((DWORD)(fMatch))

#define D2CMPPaletteSetDarkToBlack(fDark) \
	D2CMPOrdinal10011((DWORD)(fDark))

#define D2CMPPaletteSetUsable(pbUsable) \
	D2CMPOrdinal10012((DWORD)(pbUsable))

#define D2CMPCelListCreate(format, dwNumDirections, pdwKeyColor, f24Bit) \
	D2CMPOrdinal10013((DWORD)(format), (DWORD)(dwNumDirections), (DWORD)(pdwKeyColor), (DWORD)(f24Bit))

#define D2CMPCelListFree(dwHandle) \
	D2CMPOrdinal10014((DWORD)(dwHandle))

#define D2CMPCelListAddImage(dwHandle, pbImage, dwImageByteStride, dwPixelHeight, dwPixelWidth, dwOffsetX, \
		dwOffsetY, dwValue) \
	D2CMPOrdinal10015((DWORD)(dwHandle), (DWORD)(pbImage), (DWORD)(dwImageByteStride), \
		(DWORD)(dwPixelHeight), (DWORD)(dwPixelWidth), (DWORD)(dwOffsetX), (DWORD)(dwOffsetY), \
		(DWORD)(dwValue))

#define D2CMPCelListBuild(dwHandle, ppFile) \
	D2CMPOrdinal10016((DWORD)(dwHandle), (DWORD)(ppFile))

#define D2CMPCelGetNormalDirection(nNormalDirection, nNumberOfDirections) \
	D2CMPOrdinal10017((DWORD)(nNormalDirection), (DWORD)(nNumberOfDirections))

#define D2CMPCelGetGameDirection(nGameDirection, nLevelOfRotation) \
	D2CMPOrdinal10018((DWORD)(nGameDirection), (DWORD)(nLevelOfRotation))

#define D2CMPCelConvertDirection(nDirection, nStartDirs, nEndDirs, fAlternate) \
	((int)D2CMPOrdinal10019((DWORD)(nDirection), (DWORD)(nStartDirs), (DWORD)(nEndDirs), \
		(DWORD)(fAlternate)))

#define D2CMPCelGetDirectionConversion(nFrom, nTo) \
	((int)D2CMPOrdinal10020((DWORD)(nFrom), (DWORD)(nTo)))

#define D2CMPCelGetRoundedGameDirection(nGameDirection, nLevelOfRotation) \
	D2CMPOrdinal10021((DWORD)(nGameDirection), (DWORD)(nLevelOfRotation))

#define D2CMPCelGetDirectionIndex(nGameDirection, nLevelOfRotation) \
	D2CMPOrdinal10022((DWORD)(nGameDirection), (DWORD)(nLevelOfRotation))

#define D2CMPCelFileSerialize(pFile, ppSerializedFile, pdwSize) \
	((BOOL)D2CMPOrdinal10023((DWORD)(pFile), (DWORD)(ppSerializedFile), (DWORD)(pdwSize)))

#define D2CMPCelFileLoadSerialized(pFile, ppFile, dwUnused3, dwUnused4, nSpecVersion, dwUnused6) \
	D2CMPOrdinal10024((DWORD)(pFile), (DWORD)(ppFile), (DWORD)(dwUnused3), (DWORD)(dwUnused4), \
		(DWORD)(nSpecVersion), (DWORD)(dwUnused6))

#define D2CMPCelFileConvert24Bit(pFile, ppFile, a3, dwKeyColor) \
	((BOOL)D2CMPOrdinal10025((DWORD)(pFile), (DWORD)(ppFile), (DWORD)(a3), (DWORD)(dwKeyColor)))

#define D2CMPCelFileForEachPixel(pFile, a2) \
	D2CMPOrdinal10026((DWORD)(pFile), (DWORD)(a2))

#define D2CMPCelFileFree(pCelFile) \
	D2CMPOrdinal10027((DWORD)(pCelFile))

#define D2CMPCelFileFreeSerialized(pSerializedFile) \
	D2CMPOrdinal10028((DWORD)(pSerializedFile))

#define D2CMPCelFileCheckHandle(hFile, dwUnused) \
	((void *)D2CMPOrdinal10029((DWORD)(hFile), (DWORD)(dwUnused)))

#define D2CMPCelFileConvertVersion(pOldFile, ppFile, pdwSize) \
	D2CMPOrdinal10030((DWORD)(pOldFile), (DWORD)(ppFile), (DWORD)(pdwSize))

#define D2CMPCelFileRemapColors(pCelFile, pbMap) \
	D2CMPOrdinal10031((DWORD)(pCelFile), (DWORD)(pbMap))

#define D2CMPCelFileFreeHardware(ptFile) \
	((BOOL)D2CMPOrdinal10032((DWORD)(ptFile)))

#define D2CMPCelContextDraw(pCelContext, nX, nY, nClipLeft, nClipRight, pDestSurface, nSurfaceHeight, \
		dwStride, pbColorMap, pbBlend, pbPreMap) \
	D2CMPOrdinal10033((DWORD)(pCelContext), (DWORD)(nX), (DWORD)(nY), (DWORD)(nClipLeft), \
		(DWORD)(nClipRight), (DWORD)(pDestSurface), (DWORD)(nSurfaceHeight), (DWORD)(dwStride), \
		(DWORD)(pbColorMap), (DWORD)(pbBlend), (DWORD)(pbPreMap))

#define D2CMPCelContextDrawRows(pCelContext, nX, nY, nClipLeft, nClipRight, pDestSurface, nSurfaceHeight, \
		dwStride, pbColorMap, pbBlend, nFromBottom, nRows, pbPreMap) \
	D2CMPOrdinal10034((DWORD)(pCelContext), (DWORD)(nX), (DWORD)(nY), (DWORD)(nClipLeft), \
		(DWORD)(nClipRight), (DWORD)(pDestSurface), (DWORD)(nSurfaceHeight), (DWORD)(dwStride), \
		(DWORD)(pbColorMap), (DWORD)(pbBlend), (DWORD)(nFromBottom), (DWORD)(nRows), (DWORD)(pbPreMap))

#define D2CMPCelContextDrawShadow(pCelContext, nX, nY, nClipLeft, nClipRight, pDestSurface, nSurfaceHeight, \
		dwStride, pbTable, dwColor) \
	D2CMPOrdinal10035((DWORD)(pCelContext), (DWORD)(nX), (DWORD)(nY), (DWORD)(nClipLeft), \
		(DWORD)(nClipRight), (DWORD)(pDestSurface), (DWORD)(nSurfaceHeight), (DWORD)(dwStride), \
		(DWORD)(pbTable), (DWORD)(dwColor))

#define D2CMPCelContextGetCel(pCelContext) \
	((void *)D2CMPOrdinal10036((DWORD)(pCelContext)))

#define D2CMPCelGetWidth(pCel) \
	D2CMPOrdinal10037((DWORD)(pCel))

#define D2CMPCelGetHeight(pCel) \
	D2CMPOrdinal10038((DWORD)(pCel))

#define D2CMPCelGetOffsetX(pCel) \
	((int)D2CMPOrdinal10039((DWORD)(pCel)))

#define D2CMPCelGetOffsetY(pCel) \
	((int)D2CMPOrdinal10040((DWORD)(pCel)))

#define D2CMPCelGetData(ptCel) \
	((BYTE *)D2CMPOrdinal10041((DWORD)(ptCel)))

#define D2CMPCelFileCopy24Bit(pCelFile, nDirection, nFrame, pbDest) \
	((BOOL)D2CMPOrdinal10042((DWORD)(pCelFile), (DWORD)(nDirection), (DWORD)(nFrame), (DWORD)(pbDest)))

#define D2CMPCelGetSlot(ptCel) \
	((void *)D2CMPOrdinal10043((DWORD)(ptCel)))

#define D2CMPCelSetHardwareFree(pfnFree) \
	D2CMPOrdinal10044((DWORD)(pfnFree))

#define D2CMPCelGetHardwareFree() \
	D2CMPOrdinal10045()

#define D2CMPCelFileGetNumFrames(pFile) \
	D2CMPOrdinal10046((DWORD)(pFile))

#define D2CMPCelFileGetNumDirections(pFile) \
	D2CMPOrdinal10047((DWORD)(pFile))

#define D2CMPFastCmpWarp(pbSurface, nPitch) \
	D2CMPOrdinal10048((DWORD)(pbSurface), (DWORD)(nPitch))

#define D2CMPFastCmpToggleDecode() \
	D2CMPOrdinal10049()

#define D2CMPSpriteCacheMakeFileName(pszFileName, fDcc, pszFormat, pArgs) \
	D2CMPOrdinal10050((DWORD)(pszFileName), (DWORD)(fDcc), (DWORD)(pszFormat), (DWORD)(pArgs))

#define D2CMPSpriteCacheGetExtension(fDcc) \
	((const char *)D2CMPOrdinal10051((DWORD)(fDcc)))

#define D2CMPSpriteCacheInitAll(pPool, dwGfxMax, dwCelDataMax, dwMemory) \
	D2CMPOrdinal10052((DWORD)(pPool), (DWORD)(dwGfxMax), (DWORD)(dwCelDataMax), (DWORD)(dwMemory))

#define D2CMPSpriteCacheShutdown(fFree) \
	D2CMPOrdinal10053((DWORD)(fFree))

#define D2CMPSpriteCacheLoadSettings(fAllow) \
	D2CMPOrdinal10054((DWORD)(fAllow))

#define D2CMPSpriteCacheGetContextCel(ptCelContext, fPoll, fAsyncAhead) \
	((BOOL)D2CMPOrdinal10055((DWORD)(ptCelContext), (DWORD)(fPoll), (DWORD)(fAsyncAhead)))

#define D2CMPSpriteCacheLoadContext(ptCelContext) \
	((BOOL)D2CMPOrdinal10056((DWORD)(ptCelContext)))

#define D2CMPSpriteCacheCountLoading() \
	((int)D2CMPOrdinal10057())

#define D2CMPSpriteCacheIsLoading(dwValue0, dwMode, dwClass) \
	((BOOL)D2CMPOrdinal10058((DWORD)(dwValue0), (DWORD)(dwMode), (DWORD)(dwClass)))

#define D2CMPSpriteCacheGetAllocated() \
	D2CMPOrdinal10060()

#define D2CMPSpriteCacheGetMax() \
	D2CMPOrdinal10061()

#define D2CMPSpriteCacheGetCelDataAllocated() \
	D2CMPOrdinal10063()

#define D2CMPSpriteCacheGetCelDataMax() \
	D2CMPOrdinal10064()

#define D2CMPTileListCreate(dwFirst, dwSecond) \
	((void *)D2CMPOrdinal10065((DWORD)(dwFirst), (DWORD)(dwSecond)))

#define D2CMPTileListFree(ptList) \
	D2CMPOrdinal10066((DWORD)(ptList))

#define D2CMPTileListAddTile(ptList, wValue16, wValue14, dwValue24, dwValue28, dwValue2C, dwValue30, \
		dwValue10, pbValue38) \
	((void *)D2CMPOrdinal10067((DWORD)(ptList), (DWORD)(wValue16), (DWORD)(wValue14), (DWORD)(dwValue24), \
		(DWORD)(dwValue28), (DWORD)(dwValue2C), (DWORD)(dwValue30), (DWORD)(dwValue10), (DWORD)(pbValue38)))

#define D2CMPTileListTileAddBlock(hTileAdd, ptImage, nPitch, nOffsetX, nOffsetY) \
	D2CMPOrdinal10068((DWORD)(hTileAdd), (DWORD)(ptImage), (DWORD)(nPitch), (DWORD)(nOffsetX), \
		(DWORD)(nOffsetY))

#define D2CMPTileListTileCut(ptTileAdd, ptImage) \
	D2CMPOrdinal10069((DWORD)(ptTileAdd), (DWORD)(ptImage))

#define D2CMPTileListCreateProject(ptTileList, pptProject) \
	D2CMPOrdinal10070((DWORD)(ptTileList), (DWORD)(pptProject))

#define D2CMPTileProjectSerialize(ptProject, ptptData, ptnSize) \
	D2CMPOrdinal10071((DWORD)(ptProject), (DWORD)(ptptData), (DWORD)(ptnSize))

#define D2CMPTileProjectLoad(ptProject, pptProject, pszName) \
	((DWORD (__stdcall *)(void *, void *, void *))D2CMPOrdinal10072)((void *)(ptProject), \
		(void *)(pptProject), (void *)(pszName))

#define D2CMPTileProjectFree(ptProject, bKeep) \
	((DWORD (__stdcall *)(void *, DWORD))D2CMPOrdinal10073)((void *)(ptProject), (DWORD)(bKeep))

#define D2CMPTileProjectForEachPixel(ptProject, a2) \
	D2CMPOrdinal10074((DWORD)(ptProject), (DWORD)(a2))

#define D2CMPTileProjectConvert24Bit(ptProject24, pptProject8, a3) \
	D2CMPOrdinal10075((DWORD)(ptProject24), (DWORD)(pptProject8), (DWORD)(a3))

#define D2CMPTileLibrarySetHardwareFree(pfnFree) \
	D2CMPOrdinal10076((DWORD)(pfnFree))

#define D2CMPTileGetBaseTile(hTile) \
	D2CMPOrdinal10077((DWORD)(hTile))

#define D2CMPTileGetStyle(hTile) \
	D2CMPOrdinal10078((DWORD)(hTile))

#define D2CMPTileGetFlags(hTile) \
	D2CMPOrdinal10079((DWORD)(hTile))

#define D2CMPTileGetWidth(hTile) \
	D2CMPOrdinal10080((DWORD)(hTile))

#define D2CMPTileGetRarity(hTile) \
	D2CMPOrdinal10081((DWORD)(hTile))

#define D2CMPTileGetSequence(hTile) \
	D2CMPOrdinal10082((DWORD)(hTile))

#define D2CMPTileGetFirstDword(hTile) \
	D2CMPOrdinal10083((DWORD)(hTile))

#define D2CMPTileGetHeight(hTile) \
	D2CMPOrdinal10084((DWORD)(hTile))

#define D2CMPTileGetCollisionMask(hTile) \
	((BYTE *)D2CMPOrdinal10085((DWORD)(hTile)))

#define D2CMPTileGetFifthDword(hTile) \
	D2CMPOrdinal10086((DWORD)(hTile))

#define D2CMPTileLibraryAddProject(ptTileLibrary, szName) \
	D2CMPOrdinal10087((DWORD)(ptTileLibrary), (DWORD)(szName))

#define D2CMPTileLibraryFindTiles(ptTileLibrary, dwValue1, dwValue2, dwValue3, pptTiles, nMax) \
	((int)((DWORD (__stdcall *)(void *, DWORD, DWORD, DWORD, void *, \
		DWORD))D2CMPOrdinal10088)((void *)(ptTileLibrary), (DWORD)(dwValue1), (DWORD)(dwValue2), \
		(DWORD)(dwValue3), (void *)(pptTiles), (DWORD)(nMax)))

#define D2CMPTileCacheInit(pPool, dwSize) \
	D2CMPOrdinal10089((DWORD)(pPool), (DWORD)(dwSize))

#define D2CMPTileCacheShutdown(bFree) \
	D2CMPOrdinal10090((DWORD)(bFree))

#define D2CMPTileCacheLoadTile(ptTile, dwUnused) \
	((BOOL)D2CMPOrdinal10091((DWORD)(ptTile), (DWORD)(dwUnused)))

#define D2CMPTileCacheGetStats(pdwSize, pdwAllocs, pdwUses, pdwPercent) \
	D2CMPOrdinal10092((DWORD)(pdwSize), (DWORD)(pdwAllocs), (DWORD)(pdwUses), (DWORD)(pdwPercent))

#define D2CMPTileCacheResetStats() \
	D2CMPOrdinal10093()

#define D2CMPTileProjectsFreeAll() \
	D2CMPOrdinal10094()

#define D2CMPTileProjectsGet(pptHash, szName) \
	D2CMPOrdinal10095((DWORD)(pptHash), (DWORD)(szName))

#define D2CMPPalShiftLoad() \
	D2CMPOrdinal10096()

#define D2CMPPalShiftSetLevel(nLevel) \
	D2CMPOrdinal10097((DWORD)(nLevel))

#define D2CMPPalShiftGetTransform(eClass, eTransform) \
	((BYTE *)D2CMPOrdinal10098((DWORD)(eClass), (DWORD)(eTransform)))

#define D2CMPSpriteCacheAbandonLoads() \
	D2CMPOrdinal10099()

#define D2CMPPaletteCreateTablesRaw D2CMPOrdinal10000
#define D2CMPPaletteCreateBlendTableRaw D2CMPOrdinal10001
#define D2CMPPaletteFreeTableRaw D2CMPOrdinal10002
#define D2CMPPaletteCreateAddTableRaw D2CMPOrdinal10003
#define D2CMPPaletteFindNearestRaw D2CMPOrdinal10004
#define D2CMPPaletteFindFarthestRaw D2CMPOrdinal10005
#define D2CMPPaletteLoadRaw D2CMPOrdinal10006
#define D2CMPPaletteMatchColorRaw D2CMPOrdinal10007
#define D2CMPPaletteQuantizeRaw D2CMPOrdinal10008
#define D2CMPPaletteSetKeyColorRaw D2CMPOrdinal10009
#define D2CMPPaletteSetMatchKeyColorRaw D2CMPOrdinal10010
#define D2CMPPaletteSetDarkToBlackRaw D2CMPOrdinal10011
#define D2CMPPaletteSetUsableRaw D2CMPOrdinal10012
#define D2CMPCelListCreateRaw D2CMPOrdinal10013
#define D2CMPCelListFreeRaw D2CMPOrdinal10014
#define D2CMPCelListAddImageRaw D2CMPOrdinal10015
#define D2CMPCelListBuildRaw D2CMPOrdinal10016
#define D2CMPCelGetNormalDirectionRaw D2CMPOrdinal10017
#define D2CMPCelGetGameDirectionRaw D2CMPOrdinal10018
#define D2CMPCelConvertDirectionRaw D2CMPOrdinal10019
#define D2CMPCelGetDirectionConversionRaw D2CMPOrdinal10020
#define D2CMPCelGetRoundedGameDirectionRaw D2CMPOrdinal10021
#define D2CMPCelGetDirectionIndexRaw D2CMPOrdinal10022
#define D2CMPCelFileSerializeRaw D2CMPOrdinal10023
#define D2CMPCelFileLoadSerializedRaw D2CMPOrdinal10024
#define D2CMPCelFileConvert24BitRaw D2CMPOrdinal10025
#define D2CMPCelFileForEachPixelRaw D2CMPOrdinal10026
#define D2CMPCelFileFreeRaw D2CMPOrdinal10027
#define D2CMPCelFileFreeSerializedRaw D2CMPOrdinal10028
#define D2CMPCelFileCheckHandleRaw D2CMPOrdinal10029
#define D2CMPCelFileConvertVersionRaw D2CMPOrdinal10030
#define D2CMPCelFileRemapColorsRaw D2CMPOrdinal10031
#define D2CMPCelFileFreeHardwareRaw D2CMPOrdinal10032
#define D2CMPCelContextDrawRaw D2CMPOrdinal10033
#define D2CMPCelContextDrawRowsRaw D2CMPOrdinal10034
#define D2CMPCelContextDrawShadowRaw D2CMPOrdinal10035
#define D2CMPCelContextGetCelRaw D2CMPOrdinal10036
#define D2CMPCelGetWidthRaw D2CMPOrdinal10037
#define D2CMPCelGetHeightRaw D2CMPOrdinal10038
#define D2CMPCelGetOffsetXRaw D2CMPOrdinal10039
#define D2CMPCelGetOffsetYRaw D2CMPOrdinal10040
#define D2CMPCelGetDataRaw D2CMPOrdinal10041
#define D2CMPCelFileCopy24BitRaw D2CMPOrdinal10042
#define D2CMPCelGetSlotRaw D2CMPOrdinal10043
#define D2CMPCelSetHardwareFreeRaw D2CMPOrdinal10044
#define D2CMPCelGetHardwareFreeRaw D2CMPOrdinal10045
#define D2CMPCelFileGetNumFramesRaw D2CMPOrdinal10046
#define D2CMPCelFileGetNumDirectionsRaw D2CMPOrdinal10047
#define D2CMPFastCmpWarpRaw D2CMPOrdinal10048
#define D2CMPFastCmpToggleDecodeRaw D2CMPOrdinal10049
#define D2CMPSpriteCacheMakeFileNameRaw D2CMPOrdinal10050
#define D2CMPSpriteCacheGetExtensionRaw D2CMPOrdinal10051
#define D2CMPSpriteCacheInitAllRaw D2CMPOrdinal10052
#define D2CMPSpriteCacheShutdownRaw D2CMPOrdinal10053
#define D2CMPSpriteCacheLoadSettingsRaw D2CMPOrdinal10054
#define D2CMPSpriteCacheGetContextCelRaw D2CMPOrdinal10055
#define D2CMPSpriteCacheLoadContextRaw D2CMPOrdinal10056
#define D2CMPSpriteCacheCountLoadingRaw D2CMPOrdinal10057
#define D2CMPSpriteCacheIsLoadingRaw D2CMPOrdinal10058
#define D2CMPSpriteCacheNothingRaw D2CMPOrdinal10059
#define D2CMPSpriteCacheGetAllocatedRaw D2CMPOrdinal10060
#define D2CMPSpriteCacheGetMaxRaw D2CMPOrdinal10061
#define D2CMPSpriteCacheCelDataNothingRaw D2CMPOrdinal10062
#define D2CMPSpriteCacheGetCelDataAllocatedRaw D2CMPOrdinal10063
#define D2CMPSpriteCacheGetCelDataMaxRaw D2CMPOrdinal10064
#define D2CMPTileListCreateRaw D2CMPOrdinal10065
#define D2CMPTileListFreeRaw D2CMPOrdinal10066
#define D2CMPTileListAddTileRaw D2CMPOrdinal10067
#define D2CMPTileListTileAddBlockRaw D2CMPOrdinal10068
#define D2CMPTileListTileCutRaw D2CMPOrdinal10069
#define D2CMPTileListCreateProjectRaw D2CMPOrdinal10070
#define D2CMPTileProjectSerializeRaw D2CMPOrdinal10071
#define D2CMPTileProjectLoadRaw D2CMPOrdinal10072
#define D2CMPTileProjectFreeRaw D2CMPOrdinal10073
#define D2CMPTileProjectForEachPixelRaw D2CMPOrdinal10074
#define D2CMPTileProjectConvert24BitRaw D2CMPOrdinal10075
#define D2CMPTileLibrarySetHardwareFreeRaw D2CMPOrdinal10076
#define D2CMPTileGetBaseTileRaw D2CMPOrdinal10077
#define D2CMPTileGetStyleRaw D2CMPOrdinal10078
#define D2CMPTileGetFlagsRaw D2CMPOrdinal10079
#define D2CMPTileGetWidthRaw D2CMPOrdinal10080
#define D2CMPTileGetRarityRaw D2CMPOrdinal10081
#define D2CMPTileGetSequenceRaw D2CMPOrdinal10082
#define D2CMPTileGetFirstDwordRaw D2CMPOrdinal10083
#define D2CMPTileGetHeightRaw D2CMPOrdinal10084
#define D2CMPTileGetCollisionMaskRaw D2CMPOrdinal10085
#define D2CMPTileGetFifthDwordRaw D2CMPOrdinal10086
#define D2CMPTileLibraryAddProjectRaw D2CMPOrdinal10087
#define D2CMPTileLibraryFindTilesRaw D2CMPOrdinal10088
#define D2CMPTileCacheInitRaw D2CMPOrdinal10089
#define D2CMPTileCacheShutdownRaw D2CMPOrdinal10090
#define D2CMPTileCacheLoadTileRaw D2CMPOrdinal10091
#define D2CMPTileCacheGetStatsRaw D2CMPOrdinal10092
#define D2CMPTileCacheResetStatsRaw D2CMPOrdinal10093
#define D2CMPTileProjectsFreeAllRaw D2CMPOrdinal10094
#define D2CMPTileProjectsGetRaw D2CMPOrdinal10095
#define D2CMPPalShiftLoadRaw D2CMPOrdinal10096
#define D2CMPPalShiftSetLevelRaw D2CMPOrdinal10097
#define D2CMPPalShiftGetTransformRaw D2CMPOrdinal10098
#define D2CMPSpriteCacheAbandonLoadsRaw D2CMPOrdinal10099

#endif
