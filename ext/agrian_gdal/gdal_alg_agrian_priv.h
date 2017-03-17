#ifndef GDAL_ALG_AGRIAN_PRIV_H_INCLUDED
#define GDAL_ALG_AGRIAN_PRIV_H_INCLUDED

#include "gdal_alg_priv.h"

CPL_C_START

typedef enum {
  GRMA_Replace_Agrian = 0,
  GRMA_Add_Agrian = 1,
  GRMA_Max_Agrian = 2,
  GRMA_Weighted_Agrian = 3,
  GRMA_Count_Pixels_Agrian = 4
} GDALRasterMergeAlg_Agrian;

typedef struct {
  unsigned char *pabyChunkBuf;
  int nXSize;
  int nYSize;
  int nBands;
  GDALDataType eType;
  double *padfBurnValue;
  GDALBurnValueSrc eBurnValueSource;
  GDALRasterMergeAlg_Agrian eMergeAlg;
  unsigned int *totalPixelCount;
  unsigned int *dataPixelCount;
} GDALRasterizeInfo_Agrian;

/************************************************************************/
/*      Low level rasterizer API.                                       */
/************************************************************************/

void GDALdllImagePoint_Agrian(int nRasterXSize, int nRasterYSize,
                              int nPartCount, int *panPartSize, double *padfX,
                              double *padfY, double *padfVariant,
                              llPointFunc pfnPointFunc, void *pCBData);

void GDALdllImageLine_Agrian(int nRasterXSize, int nRasterYSize, int nPartCount,
                             int *panPartSize, double *padfX, double *padfY,
                             double *padfVariant, llPointFunc pfnPointFunc,
                             void *pCBData);

void GDALdllImageLineAllTouched_Agrian(int nRasterXSize, int nRasterYSize,
                                       int nPartCount, int *panPartSize,
                                       double *padfX, double *padfY,
                                       double *padfVariant,
                                       llPointFunc pfnPointFunc, void *pCBData);

void GDALdllImageFilledPolygon_Agrian(int nRasterXSize, int nRasterYSize,
                                      int nPartCount, int *panPartSize,
                                      double *padfX, double *padfY,
                                      double *padfVariant,
                                      llScanlineFunc pfnScanlineFunc,
                                      void *pCBData);

CPL_C_END

#endif /* ndef GDAL_ALG_AGRIAN_PRIV_H_INCLUDED */
