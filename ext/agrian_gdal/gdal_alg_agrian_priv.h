/******************************************************************************
 * $Id$
 *
 * Project:  GDAL Image Processing Algorithms
 * Purpose:  Prototypes and definitions for various GDAL based algorithms:
 *           private declarations.
 * Author:   Andrey Kiselev, dron@ak4719.spb.edu
 *
 ******************************************************************************
 * Copyright (c) 2008, Andrey Kiselev <dron@ak4719.spb.edu>
 * Copyright (c) 2010-2013, Even Rouault <even dot rouault at mines-paris dot
 *org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

/******************************************************************************
 * Extracted from gdal_alh_priv.  Changes noted in comments.
 * See original source at:
 * https://github.com/OSGeo/gdal/blob/tags/1.11.5/gdal/alg/gdal_alg_priv.h
 ****************************************************************************/

#ifndef GDAL_ALG_AGRIAN_PRIV_H_INCLUDED
#define GDAL_ALG_AGRIAN_PRIV_H_INCLUDED

#include "gdal_alg_priv.h"

CPL_C_START

typedef enum {
  GRMA_Replace_Agrian = 0,
  GRMA_Add_Agrian = 1,
  /* begin new types */
  GRMA_Max_Agrian = 2,
  GRMA_Weighted_Agrian = 3,
  GRMA_Count_Pixels_Agrian = 4
  /* end new types */
} GDALRasterMergeAlg_Agrian;

typedef struct {
  unsigned char *pabyChunkBuf;
  int nXSize;
  int nYSize;
  int nBands;
  GDALDataType eType;
  double *padfBurnValue;
  GDALBurnValueSrc eBurnValueSource;
  /* begin new attributes */
  GDALRasterMergeAlg_Agrian eMergeAlg;
  unsigned int *totalPixelCount;
  unsigned int *dataPixelCount;
  /* end new attributes */
} GDALRasterizeInfo_Agrian;

/************************************************************************/
/*      Low level rasterizer API. Same as GDAL functions, but with      */
/*      added `_Agrian` suffix.                                         */
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
