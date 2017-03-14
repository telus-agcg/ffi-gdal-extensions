/******************************************************************************
 * $Id$
 *
 * Project:  GDAL
 * Purpose:  Vector rasterization.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2005, Frank Warmerdam <warmerdam@pobox.com>
 * Copyright (c) 2008-2013, Even Rouault <even dot rouault at mines-paris dot
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

#include "gdal_alg_agrian.h"
#include "gdal_alg_agrian_priv.h"
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogr_spatialref.h"

#include "ogrsf_frmts.h"

/************************************************************************/
/*                           gvCountPixels()                           */
/************************************************************************/

void gvCountPixels(void *pCBData, int nY, int nXStart, int nXEnd,
                   double dfVariant) {
  GDALRasterizeInfo_Agrian *psInfo = (GDALRasterizeInfo_Agrian *)pCBData;

  if (nXStart > nXEnd)
    return;

  unsigned char *pabyInsert;
  double *padfInsert;

  if (psInfo->eType == GDT_Byte) {
    pabyInsert = psInfo->pabyChunkBuf + nY * psInfo->nXSize + nXStart;
  } else {
    padfInsert =
        ((double *)psInfo->pabyChunkBuf) + nY * psInfo->nXSize + nXStart;
  }

  CPLAssert(nY >= 0 && nY < psInfo->nYSize);
  CPLAssert(nXStart <= nXEnd);
  CPLAssert(nXStart < psInfo->nXSize);
  CPLAssert(nXEnd >= 0);

  if (nXStart < 0)
    nXStart = 0;
  if (nXEnd >= psInfo->nXSize)
    nXEnd = psInfo->nXSize - 1;

  int nPixels = nXEnd - nXStart + 1;

  *(psInfo->totalPixelCount) += nPixels;

  if (psInfo->eType == GDT_Byte) {
    while (nPixels-- > 0) {
      if (*(pabyInsert++) != (unsigned char)0) {
        *(psInfo->dataPixelCount) += 1;
      }
    }
  } else {
    while (nPixels-- > 0) {
      if (*(padfInsert++) != 0.0) {
        *(psInfo->dataPixelCount) += 1;
      }
    }
  }
}

void gvBurnScanline_Agrian(void *pCBData, int nY, int nXStart, int nXEnd,
                           double dfVariant)

{
  GDALRasterizeInfo_Agrian *psInfo = (GDALRasterizeInfo_Agrian *)pCBData;
  int iBand;

  if (nXStart > nXEnd)
    return;

  CPLAssert(nY >= 0 && nY < psInfo->nYSize);
  CPLAssert(nXStart <= nXEnd);
  CPLAssert(nXStart < psInfo->nXSize);
  CPLAssert(nXEnd >= 0);

  if (nXStart < 0)
    nXStart = 0;
  if (nXEnd >= psInfo->nXSize)
    nXEnd = psInfo->nXSize - 1;

  if (psInfo->eType == GDT_Byte) {
    for (iBand = 0; iBand < psInfo->nBands; iBand++) {
      unsigned char *pabyInsert;
      unsigned char nBurnValue =
          (unsigned char)(psInfo->padfBurnValue[iBand] +
                          ((psInfo->eBurnValueSource == GBV_UserBurnValue)
                               ? 0
                               : dfVariant));

      pabyInsert = psInfo->pabyChunkBuf +
                   iBand * psInfo->nXSize * psInfo->nYSize +
                   nY * psInfo->nXSize + nXStart;

      if (psInfo->eMergeAlg == GRMA_Add_Agrian) {
        int nPixels = nXEnd - nXStart + 1;
        while (nPixels-- > 0)
          *(pabyInsert++) += nBurnValue;
      } else if (psInfo->eMergeAlg == GRMA_Max_Agrian) {
        int nPixels = nXEnd - nXStart + 1;
        while (nPixels-- > 0) {
          *(pabyInsert) = std::max(nBurnValue, *(pabyInsert));
          pabyInsert++;
        }
      } else if (psInfo->eMergeAlg == GRMA_Weighted_Agrian) {
        int nPixels = nXEnd - nXStart + 1;
        while (nPixels-- > 0) {
          if (*(pabyInsert) == 0)
            *(pabyInsert) = nBurnValue;
          pabyInsert++;
        }
      } else {
        memset(pabyInsert, nBurnValue, nXEnd - nXStart + 1);
      }
    }
  } else if (psInfo->eType == GDT_Float64) {
    for (iBand = 0; iBand < psInfo->nBands; iBand++) {
      int nPixels = nXEnd - nXStart + 1;
      double *padfInsert;
      double dfBurnValue =
          (psInfo->padfBurnValue[iBand] +
           ((psInfo->eBurnValueSource == GBV_UserBurnValue) ? 0 : dfVariant));

      padfInsert = ((double *)psInfo->pabyChunkBuf) +
                   iBand * psInfo->nXSize * psInfo->nYSize +
                   nY * psInfo->nXSize + nXStart;

      if (psInfo->eMergeAlg == GRMA_Add_Agrian) {
        while (nPixels-- > 0)
          *(padfInsert++) += dfBurnValue;
      } else if (psInfo->eMergeAlg == GRMA_Max_Agrian) {
        while (nPixels-- > 0) {
          *(padfInsert++) = std::max(dfBurnValue, *(padfInsert));
        }
      } else if (psInfo->eMergeAlg == GRMA_Weighted_Agrian) {
        int nPixels = nXEnd - nXStart + 1;
        while (nPixels-- > 0) {
          if (*(padfInsert) == 0)
            *(padfInsert) = dfBurnValue;
          padfInsert++;
        }
      } else {
        while (nPixels-- > 0)
          *(padfInsert++) = dfBurnValue;
      }
    }
  } else {
    CPLAssert(0);
  }
}

/************************************************************************/
/*                            gvBurnPoint_Agrian() */
/************************************************************************/

void gvBurnPoint_Agrian(void *pCBData, int nY, int nX, double dfVariant)

{
  GDALRasterizeInfo_Agrian *psInfo = (GDALRasterizeInfo_Agrian *)pCBData;
  int iBand;

  CPLAssert(nY >= 0 && nY < psInfo->nYSize);
  CPLAssert(nX >= 0 && nX < psInfo->nXSize);

  if (psInfo->eType == GDT_Byte) {
    for (iBand = 0; iBand < psInfo->nBands; iBand++) {
      unsigned char *pbyInsert = psInfo->pabyChunkBuf +
                                 iBand * psInfo->nXSize * psInfo->nYSize +
                                 nY * psInfo->nXSize + nX;

      if (psInfo->eMergeAlg == GRMA_Add_Agrian) {
        *pbyInsert +=
            (unsigned char)(psInfo->padfBurnValue[iBand] +
                            ((psInfo->eBurnValueSource == GBV_UserBurnValue)
                                 ? 0
                                 : dfVariant));
      } else if (psInfo->eMergeAlg == GRMA_Max_Agrian) {
        *pbyInsert = std::max(
            *pbyInsert,
            (unsigned char)(psInfo->padfBurnValue[iBand] +
                            ((psInfo->eBurnValueSource == GBV_UserBurnValue)
                                 ? 0
                                 : dfVariant)));
      } else {
        *pbyInsert =
            (unsigned char)(psInfo->padfBurnValue[iBand] +
                            ((psInfo->eBurnValueSource == GBV_UserBurnValue)
                                 ? 0
                                 : dfVariant));
      }
    }
  } else if (psInfo->eType == GDT_Float64) {
    for (iBand = 0; iBand < psInfo->nBands; iBand++) {
      double *pdfInsert = ((double *)psInfo->pabyChunkBuf) +
                          iBand * psInfo->nXSize * psInfo->nYSize +
                          nY * psInfo->nXSize + nX;

      if (psInfo->eMergeAlg == GRMA_Add_Agrian) {
        *pdfInsert +=
            (psInfo->padfBurnValue[iBand] +
             ((psInfo->eBurnValueSource == GBV_UserBurnValue) ? 0 : dfVariant));
      } else if (psInfo->eMergeAlg == GRMA_Max_Agrian) {
        *pdfInsert = std::max(
            *pdfInsert,
            (psInfo->padfBurnValue[iBand] +
             ((psInfo->eBurnValueSource == GBV_UserBurnValue) ? 0
                                                              : dfVariant)));
      } else {
        *pdfInsert =
            (psInfo->padfBurnValue[iBand] +
             ((psInfo->eBurnValueSource == GBV_UserBurnValue) ? 0 : dfVariant));
      }
    }
  } else {
    CPLAssert(0);
  }
}

/************************************************************************/
/*                    GDALCollectRingsFromGeometry()                    */
/************************************************************************/

static void GDALCollectRingsFromGeometry(OGRGeometry *poShape,
                                         std::vector<double> &aPointX,
                                         std::vector<double> &aPointY,
                                         std::vector<double> &aPointVariant,
                                         std::vector<int> &aPartSize,
                                         GDALBurnValueSrc eBurnValueSrc)

{
  if (poShape == NULL)
    return;

  OGRwkbGeometryType eFlatType = wkbFlatten(poShape->getGeometryType());
  int i;

  if (eFlatType == wkbPoint) {
    OGRPoint *poPoint = (OGRPoint *)poShape;
    int nNewCount = aPointX.size() + 1;

    aPointX.reserve(nNewCount);
    aPointY.reserve(nNewCount);
    aPointX.push_back(poPoint->getX());
    aPointY.push_back(poPoint->getY());
    aPartSize.push_back(1);
    if (eBurnValueSrc != GBV_UserBurnValue) {
      /*switch( eBurnValueSrc )
      {
      case GBV_Z:*/
      aPointVariant.reserve(nNewCount);
      aPointVariant.push_back(poPoint->getZ());
      /*break;
  case GBV_M:
      aPointVariant.reserve( nNewCount );
      aPointVariant.push_back( poPoint->getM() );
  }*/
    }
  } else if (eFlatType == wkbLineString) {
    OGRLineString *poLine = (OGRLineString *)poShape;
    int nCount = poLine->getNumPoints();
    int nNewCount = aPointX.size() + nCount;

    aPointX.reserve(nNewCount);
    aPointY.reserve(nNewCount);
    if (eBurnValueSrc != GBV_UserBurnValue)
      aPointVariant.reserve(nNewCount);
    for (i = nCount - 1; i >= 0; i--) {
      aPointX.push_back(poLine->getX(i));
      aPointY.push_back(poLine->getY(i));
      if (eBurnValueSrc != GBV_UserBurnValue) {
        /*switch( eBurnValueSrc )
        {
            case GBV_Z:*/
        aPointVariant.push_back(poLine->getZ(i));
        /*break;
    case GBV_M:
        aPointVariant.push_back( poLine->getM(i) );
}*/
      }
    }
    aPartSize.push_back(nCount);
  } else if (EQUAL(poShape->getGeometryName(), "LINEARRING")) {
    OGRLinearRing *poRing = (OGRLinearRing *)poShape;
    int nCount = poRing->getNumPoints();
    int nNewCount = aPointX.size() + nCount;

    aPointX.reserve(nNewCount);
    aPointY.reserve(nNewCount);
    if (eBurnValueSrc != GBV_UserBurnValue)
      aPointVariant.reserve(nNewCount);
    for (i = nCount - 1; i >= 0; i--) {
      aPointX.push_back(poRing->getX(i));
      aPointY.push_back(poRing->getY(i));
    }
    if (eBurnValueSrc != GBV_UserBurnValue) {
      /*switch( eBurnValueSrc )
      {
      case GBV_Z:*/
      aPointVariant.push_back(poRing->getZ(i));
      /*break;
  case GBV_M:
      aPointVariant.push_back( poRing->getM(i) );
  }*/
    }
    aPartSize.push_back(nCount);
  } else if (eFlatType == wkbPolygon) {
    OGRPolygon *poPolygon = (OGRPolygon *)poShape;

    GDALCollectRingsFromGeometry(poPolygon->getExteriorRing(), aPointX, aPointY,
                                 aPointVariant, aPartSize, eBurnValueSrc);

    for (i = 0; i < poPolygon->getNumInteriorRings(); i++)
      GDALCollectRingsFromGeometry(poPolygon->getInteriorRing(i), aPointX,
                                   aPointY, aPointVariant, aPartSize,
                                   eBurnValueSrc);
  }

  else if (eFlatType == wkbMultiPoint || eFlatType == wkbMultiLineString ||
           eFlatType == wkbMultiPolygon || eFlatType == wkbGeometryCollection) {
    OGRGeometryCollection *poGC = (OGRGeometryCollection *)poShape;

    for (i = 0; i < poGC->getNumGeometries(); i++)
      GDALCollectRingsFromGeometry(poGC->getGeometryRef(i), aPointX, aPointY,
                                   aPointVariant, aPartSize, eBurnValueSrc);
  } else {
    CPLDebug("GDAL", "Rasterizer ignoring non-polygonal geometry.");
  }
}

/************************************************************************/
/*                       gv_rasterize_one_shape_agrian() */
/************************************************************************/
static void gv_rasterize_one_shape_agrian(
    unsigned char *pabyChunkBuf, int nYOff, int nXSize, int nYSize, int nBands,
    GDALDataType eType, int bAllTouched, OGRGeometry *poShape,
    double *padfBurnValue, GDALBurnValueSrc eBurnValueSrc,
    GDALRasterMergeAlg_Agrian eMergeAlg, GDALTransformerFunc pfnTransformer,
    void *pTransformArg, unsigned int *totalPixelCount,
    unsigned int *dataPixelCount)

{
  GDALRasterizeInfo_Agrian sInfo;

  if (poShape == NULL)
    return;

  sInfo.nXSize = nXSize;
  sInfo.nYSize = nYSize;
  sInfo.nBands = nBands;
  sInfo.pabyChunkBuf = pabyChunkBuf;
  sInfo.eType = eType;
  sInfo.padfBurnValue = padfBurnValue;
  sInfo.eBurnValueSource = eBurnValueSrc;
  sInfo.eMergeAlg = eMergeAlg;
  sInfo.dataPixelCount = dataPixelCount;
  sInfo.totalPixelCount = totalPixelCount;

  /* -------------------------------------------------------------------- */
  /*      Transform polygon geometries into a set of rings and a part     */
  /*      size list.                                                      */
  /* -------------------------------------------------------------------- */
  std::vector<double> aPointX;
  std::vector<double> aPointY;
  std::vector<double> aPointVariant;
  std::vector<int> aPartSize;

  GDALCollectRingsFromGeometry(poShape, aPointX, aPointY, aPointVariant,
                               aPartSize, eBurnValueSrc);

  /* -------------------------------------------------------------------- */
  /*      Transform points if needed.                                     */
  /* -------------------------------------------------------------------- */
  if (pfnTransformer != NULL) {
    int *panSuccess = (int *)CPLCalloc(sizeof(int), aPointX.size());

    // TODO: we need to add all appropriate error checking at some point.
    pfnTransformer(pTransformArg, FALSE, aPointX.size(), &(aPointX[0]),
                   &(aPointY[0]), NULL, panSuccess);
    CPLFree(panSuccess);
  }

  /* -------------------------------------------------------------------- */
  /*      Shift to account for the buffer offset of this buffer.          */
  /* -------------------------------------------------------------------- */
  unsigned int i;

  for (i = 0; i < aPointY.size(); i++)
    aPointY[i] -= nYOff;

  /* -------------------------------------------------------------------- */
  /*      Perform the rasterization.                                      */
  /*      According to the C++ Standard/23.2.4, elements of a vector are  */
  /*      stored in continuous memory block.                              */
  /* -------------------------------------------------------------------- */

  // TODO - mloskot: Check if vectors are empty, otherwise it may
  // lead to undefined behavior by returning non-referencable pointer.
  // if (!aPointX.empty())
  //    /* fill polygon */
  // else
  //    /* How to report this problem? */
  switch (wkbFlatten(poShape->getGeometryType())) {
  case wkbPoint:
  case wkbMultiPoint:
    GDALdllImagePoint(
        sInfo.nXSize, nYSize, aPartSize.size(), &(aPartSize[0]), &(aPointX[0]),
        &(aPointY[0]),
        (eBurnValueSrc == GBV_UserBurnValue) ? NULL : &(aPointVariant[0]),
        gvBurnPoint_Agrian, &sInfo);
    break;
  case wkbLineString:
  case wkbMultiLineString: {
    if (bAllTouched)
      GDALdllImageLineAllTouched(
          sInfo.nXSize, nYSize, aPartSize.size(), &(aPartSize[0]),
          &(aPointX[0]), &(aPointY[0]),
          (eBurnValueSrc == GBV_UserBurnValue) ? NULL : &(aPointVariant[0]),
          gvBurnPoint_Agrian, &sInfo);
    else
      GDALdllImageLine(
          sInfo.nXSize, nYSize, aPartSize.size(), &(aPartSize[0]),
          &(aPointX[0]), &(aPointY[0]),
          (eBurnValueSrc == GBV_UserBurnValue) ? NULL : &(aPointVariant[0]),
          gvBurnPoint_Agrian, &sInfo);
  } break;

  default: {
    if (eMergeAlg == GRMA_Count_Pixels_Agrian)
      GDALdllImageFilledPolygon(
          sInfo.nXSize, nYSize, aPartSize.size(), &(aPartSize[0]),
          &(aPointX[0]), &(aPointY[0]),
          (eBurnValueSrc == GBV_UserBurnValue) ? NULL : &(aPointVariant[0]),
          gvCountPixels, &sInfo);

    GDALdllImageFilledPolygon(
        sInfo.nXSize, nYSize, aPartSize.size(), &(aPartSize[0]), &(aPointX[0]),
        &(aPointY[0]),
        (eBurnValueSrc == GBV_UserBurnValue) ? NULL : &(aPointVariant[0]),
        gvBurnScanline_Agrian, &sInfo);

    if (bAllTouched) {
      /* Reverting the variants to the first value because the
         polygon is filled using the variant from the first point of
         the first segment. Should be removed when the code to full
         polygons more appropriately is added. */
      if (eBurnValueSrc == GBV_UserBurnValue) {
        GDALdllImageLineAllTouched(
            sInfo.nXSize, nYSize, aPartSize.size(), &(aPartSize[0]),
            &(aPointX[0]), &(aPointY[0]), NULL, gvBurnPoint_Agrian, &sInfo);
      } else {
        unsigned int n;
        for (i = 0, n = 0; i < aPartSize.size(); i++) {
          int j;
          for (j = 0; j < aPartSize[i]; j++)
            aPointVariant[n++] = aPointVariant[0];
        }

        GDALdllImageLineAllTouched(sInfo.nXSize, nYSize, aPartSize.size(),
                                   &(aPartSize[0]), &(aPointX[0]),
                                   &(aPointY[0]), &(aPointVariant[0]),
                                   gvBurnPoint_Agrian, &sInfo);
      }
    }
  } break;
  }
}

/************************************************************************/
/*                        GDALRasterizeOptions_Agrian() */
/*                                                                      */
/*      Recognise a few rasterize options used by all three entry       */
/*      points.                                                         */
/************************************************************************/

static CPLErr
GDALRasterizeOptions_Agrian(char **papszOptions, int *pbAllTouched,
                            GDALBurnValueSrc *peBurnValueSource,
                            GDALRasterMergeAlg_Agrian *peMergeAlg) {
  *pbAllTouched = CSLFetchBoolean(papszOptions, "ALL_TOUCHED", FALSE);

  const char *pszOpt = CSLFetchNameValue(papszOptions, "BURN_VALUE_FROM");
  *peBurnValueSource = GBV_UserBurnValue;
  if (pszOpt) {
    if (EQUAL(pszOpt, "Z"))
      *peBurnValueSource = GBV_Z;
    /*else if( EQUAL(pszOpt,"M"))
        eBurnValueSource = GBV_M;*/
    else {
      CPLError(CE_Failure, CPLE_AppDefined,
               "Unrecognised value '%s' for BURN_VALUE_FROM.", pszOpt);
      return CE_Failure;
    }
  }

  /* -------------------------------------------------------------------- */
  /*      MERGE_ALG=[REPLACE]/ADD                                         */
  /* -------------------------------------------------------------------- */
  pszOpt = CSLFetchNameValue(papszOptions, "MERGE_ALG");
  if (pszOpt) {
    if (EQUAL(pszOpt, "ADD"))
      *peMergeAlg = GRMA_Add_Agrian;
    else if (EQUAL(pszOpt, "REPLACE"))
      *peMergeAlg = GRMA_Replace_Agrian;
    else if (EQUAL(pszOpt, "MAX"))
      *peMergeAlg = GRMA_Max_Agrian;
    else if (EQUAL(pszOpt, "WEIGHTED"))
      *peMergeAlg = GRMA_Weighted_Agrian;
    else {
      CPLError(CE_Failure, CPLE_AppDefined,
               "Unrecognised value '%s' for MERGE_ALG.", pszOpt);
      return CE_Failure;
    }
  }

  return CE_None;
}

/************************************************************************/
/*                        GDALCountPixelOverlaps()                         */
/************************************************************************/

/**
 * Burn geometries from the specified list of layers into raster.
 *
 * Rasterize all the geometric objects from a list of layers into a raster
 * dataset.  The layers are passed as an array of OGRLayerH handlers.
 *
 * If the geometries are in the georferenced coordinates of the raster
 * dataset, then the pfnTransform may be passed in NULL and one will be
 * derived internally from the geotransform of the dataset.  The transform
 * needs to transform the geometry locations into pixel/line coordinates
 * on the raster dataset.
 *
 * The output raster may be of any GDAL supported datatype, though currently
 * internally the burning is done either as GDT_Byte or GDT_Float32.  This
 * may be improved in the future.  An explicit list of burn values for
 * each layer for each band must be passed in.
 *
 * @param hDS output data, must be opened in update mode.
 * @param nBandCount the number of bands to be updated.
 * @param panBandList the list of bands to be updated.
 * @param nLayerCount the number of layers being passed in pahLayers array.
 * @param pahLayers the array of layers to burn in.
 * @param pfnTransformer transformation to apply to geometries to put into
 * pixel/line coordinates on raster.  If NULL a geotransform based one will
 * be created internally.
 * @param pTransformArg callback data for transformer.
 * @param padfLayerBurnValues the array of values to burn into the raster.
 * There should be nBandCount values for each layer.
 * @param papszOptions special options controlling rasterization:
 * <dl>
 * <dt>"ATTRIBUTE":</dt> <dd>Identifies an attribute field on the features to be
 * used for a burn in value. The value will be burned into all output
 * bands. If specified, padfLayerBurnValues will not be used and can be a NULL
 * pointer.</dd>
 * <dt>"CHUNKYSIZE":</dt> <dd>The height in lines of the chunk to operate on.
 * The larger the chunk size the less times we need to make a pass through all
 * the shapes. If it is not set or set to zero the default chunk size will be
 * used. Default size will be estimated based on the GDAL cache buffer size
 * using formula: cache_size_bytes/scanline_size_bytes, so the chunk will
 * not exceed the cache.</dd>
 * <dt>"ALL_TOUCHED":</dt> <dd>May be set to TRUE to set all pixels touched
 * by the line or polygons, not just those whose center is within the polygon
 * or that are selected by brezenhams line algorithm.  Defaults to FALSE.</dd>
 * <dt>"BURN_VALUE_FROM":</dt> <dd>May be set to "Z" to use the Z values of the
 * geometries. The value from padfLayerBurnValues or the attribute field value
 * is added to this before burning. In default case dfBurnValue is burned as it
 * is. This is implemented properly only for points and lines for now. Polygons
 * will be burned using the Z value from the first point. The M value may be
 * supported in the future.</dd>
 * <dt>"MERGE_ALG":</dt> <dd>May be REPLACE (the default) or ADD.  REPLACE
 * results in overwriting of value, while ADD adds the new value to the existing
 * raster, suitable for heatmaps for instance.</dd>
 * </dl>
 * @param pfnProgress the progress function to report completion.
 * @param pProgressArg callback data for progress function.
 *
 * @return CE_None on success or CE_Failure on error.
 */

CPLErr GDALCountPixelOverlaps(GDALDatasetH hDS, OGRLayer *poLayer,
                              GDALTransformerFunc pfnTransformer,
                              void *pTransformArg, int bAllTouched,
                              const char *pszYChunkSize,
                              GDALProgressFunc pfnProgress, void *pProgressArg,
                              unsigned int *nPixels, unsigned int *nDataPixels)

{
  GDALDataType eType = GDT_Byte;
  unsigned char *pabyChunkBuf;
  GDALDataset *poDS = (GDALDataset *)hDS;

  if (pfnProgress == NULL)
    pfnProgress = GDALDummyProgress;

  /* -------------------------------------------------------------------- */
  /*      Options                                                         */
  /* -------------------------------------------------------------------- */
  GDALBurnValueSrc eBurnValueSource = GBV_UserBurnValue;
  GDALRasterMergeAlg_Agrian eMergeAlg = GRMA_Count_Pixels_Agrian;

  /* -------------------------------------------------------------------- */
  /*      Establish a chunksize to operate on.  The larger the chunk      */
  /*      size the less times we need to make a pass through all the      */
  /*      shapes.                                                         */
  /* -------------------------------------------------------------------- */
  int nYChunkSize, nScanlineBytes;

  nScanlineBytes = poDS->GetRasterXSize() * (GDALGetDataTypeSize(eType) / 8);

  if (pszYChunkSize && ((nYChunkSize = atoi(pszYChunkSize))) != 0)
    ;
  else {
    GIntBig nYChunkSize64 = GDALGetCacheMax64() / nScanlineBytes;
    if (nYChunkSize64 > INT_MAX)
      nYChunkSize = INT_MAX;
    else
      nYChunkSize = (int)nYChunkSize64;
  }

  if (nYChunkSize < 1)
    nYChunkSize = 1;
  if (nYChunkSize > poDS->GetRasterYSize())
    nYChunkSize = poDS->GetRasterYSize();

  CPLDebug("GDAL", "Rasterizer operating on %d swaths of %d scanlines.",
           (poDS->GetRasterYSize() + nYChunkSize - 1) / nYChunkSize,
           nYChunkSize);
  pabyChunkBuf = (unsigned char *)VSIMalloc(nYChunkSize * nScanlineBytes);
  if (pabyChunkBuf == NULL) {
    CPLError(CE_Failure, CPLE_OutOfMemory,
             "Unable to allocate rasterization buffer.");
    return CE_Failure;
  }

  /* ==================================================================== */
  /*      Read the specified layers transforming and rasterizing           */
  /*      geometries.                                                     */
  /* ==================================================================== */
  CPLErr eErr = CE_None;

  pfnProgress(0.0, NULL, pProgressArg);

  unsigned int iShape;

  double padfBurnValues[1] = {1};

  int iBurnField = -1;

  /* -------------------------------------------------------------------- */
  /*      If the layer does not contain any features just skip it.        */
  /*      Do not force the feature count, so if driver doesn't know       */
  /*      exact number of features, go down the normal way.               */
  /* -------------------------------------------------------------------- */
  unsigned int nGeomCount = poLayer->GetFeatureCount(TRUE);

  OGRFeature *poFeat;

  poLayer->ResetReading();

  /* -------------------------------------------------------------------- */
  /*      Loop over image in designated chunks.                           */
  /* -------------------------------------------------------------------- */
  int iY;
  for (iY = 0; iY < poDS->GetRasterYSize() && eErr == CE_None;
       iY += nYChunkSize) {

    int nThisYChunkSize;

    nThisYChunkSize = nYChunkSize;
    if (nThisYChunkSize + iY > poDS->GetRasterYSize())
      nThisYChunkSize = poDS->GetRasterYSize() - iY;

    // Initialize to zero to make counting easier
    memset(pabyChunkBuf, (unsigned char)0, nYChunkSize * nScanlineBytes);

    iShape = 0;

    while ((poFeat = poLayer->GetNextFeature()) != NULL) {
      OGRGeometry *poGeom = poFeat->GetGeometryRef();

      gv_rasterize_one_shape_agrian(pabyChunkBuf, iY, poDS->GetRasterXSize(),
                                    nThisYChunkSize, 1, eType, bAllTouched,
                                    poGeom, padfBurnValues, eBurnValueSource,
                                    eMergeAlg, pfnTransformer, pTransformArg,
                                    nPixels + iShape, nDataPixels + iShape);
      iShape++;
      delete poFeat;
    }

    poLayer->ResetReading();

    if (!pfnProgress((iY + nThisYChunkSize) / ((double)poDS->GetRasterYSize()),
                     "", pProgressArg)) {
      CPLError(CE_Failure, CPLE_UserInterrupt, "User terminated");
      eErr = CE_Failure;
    }
  }

  /* -------------------------------------------------------------------- */
  /*      cleanup                                                         */
  /* -------------------------------------------------------------------- */
  VSIFree(pabyChunkBuf);

  return eErr;
}

/************************************************************************/
/*                        GDALRasterizeLayers_Agrian() */
/************************************************************************/

/**
 * Burn geometries from the specified list of layers into raster.
 *
 * Rasterize all the geometric objects from a list of layers into a raster
 * dataset.  The layers are passed as an array of OGRLayerH handlers.
 *
 * If the geometries are in the georferenced coordinates of the raster
 * dataset, then the pfnTransform may be passed in NULL and one will be
 * derived internally from the geotransform of the dataset.  The transform
 * needs to transform the geometry locations into pixel/line coordinates
 * on the raster dataset.
 *
 * The output raster may be of any GDAL supported datatype, though currently
 * internally the burning is done either as GDT_Byte or GDT_Float32.  This
 * may be improved in the future.  An explicit list of burn values for
 * each layer for each band must be passed in.
 *
 * @param hDS output data, must be opened in update mode.
 * @param nBandCount the number of bands to be updated.
 * @param panBandList the list of bands to be updated.
 * @param nLayerCount the number of layers being passed in pahLayers array.
 * @param pahLayers the array of layers to burn in.
 * @param pfnTransformer transformation to apply to geometries to put into
 * pixel/line coordinates on raster.  If NULL a geotransform based one will
 * be created internally.
 * @param pTransformArg callback data for transformer.
 * @param padfLayerBurnValues the array of values to burn into the raster.
 * There should be nBandCount values for each layer.
 * @param dfPixelDivisor Used in conjunction with GRMA_Weighted_Agrian, burn
 * value will be divided by this multplied by the number of overlapping pixels.
 * For example, If we're burning values given in pounds, we may want to provide
 * the area of the pixel in acres. There should be nBandCount values for each
 * layer.
 * @param papszOptions special options controlling rasterization:
 * <dl>
 * <dt>"ATTRIBUTE":</dt> <dd>Identifies an attribute field on the features to be
 * used for a burn in value. The value will be burned into all output
 * bands. If specified, padfLayerBurnValues will not be used and can be a NULL
 * pointer.</dd>
 * <dt>"CHUNKYSIZE":</dt> <dd>The height in lines of the chunk to operate on.
 * The larger the chunk size the less times we need to make a pass through all
 * the shapes. If it is not set or set to zero the default chunk size will be
 * used. Default size will be estimated based on the GDAL cache buffer size
 * using formula: cache_size_bytes/scanline_size_bytes, so the chunk will
 * not exceed the cache.</dd>
 * <dt>"ALL_TOUCHED":</dt> <dd>May be set to TRUE to set all pixels touched
 * by the line or polygons, not just those whose center is within the polygon
 * or that are selected by brezenhams line algorithm.  Defaults to FALSE.</dd>
 * <dt>"BURN_VALUE_FROM":</dt> <dd>May be set to "Z" to use the Z values of the
 * geometries. The value from padfLayerBurnValues or the attribute field value
 * is added to this before burning. In default case dfBurnValue is burned as it
 * is. This is implemented properly only for points and lines for now. Polygons
 * will be burned using the Z value from the first point. The M value may be
 * supported in the future.</dd>
 * <dt>"MERGE_ALG":</dt> <dd>May be REPLACE (the default) or ADD.  REPLACE
 * results in overwriting of value, while ADD adds the new value to the existing
 * raster, suitable for heatmaps for instance.</dd>
 * </dl>
 * @param pfnProgress the progress function to report completion.
 * @param pProgressArg callback data for progress function.
 *
 * @return CE_None on success or CE_Failure on error.
 */

CPLErr GDALRasterizeLayers_Agrian(
    GDALDatasetH hDS, int nBandCount, int *panBandList, int nLayerCount,
    OGRLayerH *pahLayers, GDALTransformerFunc pfnTransformer,
    void *pTransformArg, double *padfLayerBurnValues, double dfPixelDivisor,
    char **papszOptions, GDALProgressFunc pfnProgress, void *pProgressArg)

{
  GDALDataType eType;
  unsigned char *pabyChunkBuf;
  GDALDataset *poDS = (GDALDataset *)hDS;

  if (pfnProgress == NULL)
    pfnProgress = GDALDummyProgress;

  /* -------------------------------------------------------------------- */
  /*      Do some rudimentary arg checking.                               */
  /* -------------------------------------------------------------------- */
  if (nBandCount == 0 || nLayerCount == 0)
    return CE_None;

  // prototype band.
  GDALRasterBand *poBand = poDS->GetRasterBand(panBandList[0]);
  if (poBand == NULL)
    return CE_Failure;

  /* -------------------------------------------------------------------- */
  /*      Options                                                         */
  /* -------------------------------------------------------------------- */
  int bAllTouched;

  GDALBurnValueSrc eBurnValueSource;
  GDALRasterMergeAlg_Agrian eMergeAlg;
  if (GDALRasterizeOptions_Agrian(papszOptions, &bAllTouched, &eBurnValueSource,
                                  &eMergeAlg) == CE_Failure) {
    return CE_Failure;
  }

  /* -------------------------------------------------------------------- */
  /*      Establish a chunksize to operate on.  The larger the chunk      */
  /*      size the less times we need to make a pass through all the      */
  /*      shapes.                                                         */
  /* -------------------------------------------------------------------- */
  int nYChunkSize, nScanlineBytes;
  const char *pszYChunkSize = CSLFetchNameValue(papszOptions, "CHUNKYSIZE");

  if (poBand->GetRasterDataType() == GDT_Byte)
    eType = GDT_Byte;
  else
    eType = GDT_Float64;

  nScanlineBytes =
      nBandCount * poDS->GetRasterXSize() * (GDALGetDataTypeSize(eType) / 8);

  if (pszYChunkSize && ((nYChunkSize = atoi(pszYChunkSize))) != 0)
    ;
  else {
    GIntBig nYChunkSize64 = GDALGetCacheMax64() / nScanlineBytes;
    if (nYChunkSize64 > INT_MAX)
      nYChunkSize = INT_MAX;
    else
      nYChunkSize = (int)nYChunkSize64;
  }

  if (nYChunkSize < 1)
    nYChunkSize = 1;
  if (nYChunkSize > poDS->GetRasterYSize())
    nYChunkSize = poDS->GetRasterYSize();

  CPLDebug("GDAL", "Rasterizer operating on %d swaths of %d scanlines.",
           (poDS->GetRasterYSize() + nYChunkSize - 1) / nYChunkSize,
           nYChunkSize);
  pabyChunkBuf = (unsigned char *)VSIMalloc(nYChunkSize * nScanlineBytes);
  if (pabyChunkBuf == NULL) {
    CPLError(CE_Failure, CPLE_OutOfMemory,
             "Unable to allocate rasterization buffer.");
    return CE_Failure;
  }

  /* -------------------------------------------------------------------- */
  /*      Read the image once for all layers if user requested to render  */
  /*      the whole raster in single chunk.                               */
  /* -------------------------------------------------------------------- */
  if (nYChunkSize == poDS->GetRasterYSize()) {
    if (poDS->RasterIO(GF_Read, 0, 0, poDS->GetRasterXSize(), nYChunkSize,
                       pabyChunkBuf, poDS->GetRasterXSize(), nYChunkSize, eType,
                       nBandCount, panBandList, 0, 0, 0) != CE_None) {
      CPLError(CE_Failure, CPLE_OutOfMemory, "Unable to read buffer.");
      CPLFree(pabyChunkBuf);
      return CE_Failure;
    }
  }

  /* ==================================================================== */
  /*      Read the specified layers transforming and rasterizing           */
  /*      geometries.                                                     */
  /* ==================================================================== */
  CPLErr eErr = CE_None;
  int iLayer;
  const char *pszBurnAttribute = CSLFetchNameValue(papszOptions, "ATTRIBUTE");

  pfnProgress(0.0, NULL, pProgressArg);

  unsigned int iShape;

  unsigned int *nPixels;
  unsigned int *nDataPixels;

  if (eErr != CE_None)
    return eErr;

  for (iLayer = 0; iLayer < nLayerCount; iLayer++) {
    int iBurnField = -1;
    double *padfBurnValues = NULL;
    OGRLayer *poLayer = (OGRLayer *)pahLayers[iLayer];

    if (!poLayer) {
      CPLError(CE_Warning, CPLE_AppDefined,
               "Layer element number %d is NULL, skipping.\n", iLayer);
      continue;
    }

    /* -------------------------------------------------------------------- */
    /*      If the layer does not contain any features just skip it.        */
    /*      Do not force the feature count, so if driver doesn't know       */
    /*      exact number of features, go down the normal way.               */
    /* -------------------------------------------------------------------- */
    unsigned int nGeomCount = poLayer->GetFeatureCount(TRUE);

    if (nGeomCount == 0)
      continue;

    nPixels = new unsigned int[nGeomCount]();
    nDataPixels = new unsigned int[nGeomCount]();

    if (pszBurnAttribute) {
      iBurnField = poLayer->GetLayerDefn()->GetFieldIndex(pszBurnAttribute);
      if (iBurnField == -1) {
        CPLError(CE_Warning, CPLE_AppDefined,
                 "Failed to find field %s on layer %s, skipping.\n",
                 pszBurnAttribute, poLayer->GetLayerDefn()->GetName());
        delete[] nPixels;
        delete[] nDataPixels;
        continue;
      }
    } else
      padfBurnValues = padfLayerBurnValues + iLayer * nBandCount;

    /* -------------------------------------------------------------------- */
    /*      If we have no transformer, create the one from input file       */
    /*      projection. Note that each layer can be georefernced            */
    /*      separately.                                                     */
    /* -------------------------------------------------------------------- */
    int bNeedToFreeTransformer = FALSE;

    if (pfnTransformer == NULL) {
      char *pszProjection = NULL;
      bNeedToFreeTransformer = TRUE;

      OGRSpatialReference *poSRS = poLayer->GetSpatialRef();
      if (!poSRS) {
        CPLError(
            CE_Warning, CPLE_AppDefined,
            "Failed to fetch spatial reference on layer %s "
            "to build transformer, assuming matching coordinate systems.\n",
            poLayer->GetLayerDefn()->GetName());
      } else
        poSRS->exportToWkt(&pszProjection);

      pTransformArg = GDALCreateGenImgProjTransformer(NULL, pszProjection, hDS,
                                                      NULL, FALSE, 0.0, 0);
      pfnTransformer = GDALGenImgProjTransform;

      CPLFree(pszProjection);
    }

    if (eMergeAlg == GRMA_Weighted_Agrian)
      eErr = GDALCountPixelOverlaps(hDS, poLayer, pfnTransformer, pTransformArg,
                                    bAllTouched, pszYChunkSize, pfnProgress,
                                    pProgressArg, nPixels, nDataPixels);

    OGRFeature *poFeat;

    poLayer->ResetReading();

    /* -------------------------------------------------------------------- */
    /*      Loop over image in designated chunks.                           */
    /* -------------------------------------------------------------------- */
    int iY;
    for (iY = 0; iY < poDS->GetRasterYSize() && eErr == CE_None;
         iY += nYChunkSize) {
      int nThisYChunkSize;

      nThisYChunkSize = nYChunkSize;
      if (nThisYChunkSize + iY > poDS->GetRasterYSize())
        nThisYChunkSize = poDS->GetRasterYSize() - iY;

      // Only re-read image if not a single chunk is being rendered
      if (nYChunkSize < poDS->GetRasterYSize()) {
        eErr = poDS->RasterIO(GF_Read, 0, iY, poDS->GetRasterXSize(),
                              nThisYChunkSize, pabyChunkBuf,
                              poDS->GetRasterXSize(), nThisYChunkSize, eType,
                              nBandCount, panBandList, 0, 0, 0);
        if (eErr != CE_None)
          break;
      }

      double *padfAttrValues = (double *)VSIMalloc(sizeof(double) * nBandCount);

      iShape = 0;

      while ((poFeat = poLayer->GetNextFeature()) != NULL) {
        OGRGeometry *poGeom = poFeat->GetGeometryRef();

        if (pszBurnAttribute) {
          int iBand;
          double dfAttrValue;

          dfAttrValue = poFeat->GetFieldAsDouble(iBurnField);
          for (iBand = 0; iBand < nBandCount; iBand++)
            padfAttrValues[iBand] = dfAttrValue;

          padfBurnValues = padfAttrValues;
        }

        GDALRasterMergeAlg_Agrian eMergeAlgCurrent = eMergeAlg;
        if (eMergeAlg == GRMA_Weighted_Agrian) {
          // ratio is the percentage of pixels that were not yet written
          double ratio =
              (nPixels[iShape] - nDataPixels[iShape]) / (double)nPixels[iShape];

          if (ratio > 0.2) {
            double dfFinalDivisor;

            if (dfPixelDivisor > 0) {
              dfFinalDivisor =
                  dfPixelDivisor * (nPixels[iShape] - nDataPixels[iShape]);
            } else {
              dfFinalDivisor = ratio;
            }

            for (int iBand = 0; iBand < nBandCount; iBand++)
              padfAttrValues[iBand] /= dfFinalDivisor;
          } else {
            eMergeAlgCurrent = GRMA_Add_Agrian;

            if (dfPixelDivisor > 0)
              for (int iBand = 0; iBand < nBandCount; iBand++)
                padfAttrValues[iBand] /= (dfPixelDivisor * nPixels[iShape]);
          }
        }

        gv_rasterize_one_shape_agrian(
            pabyChunkBuf, iY, poDS->GetRasterXSize(), nThisYChunkSize,
            nBandCount, eType, bAllTouched, poGeom, padfBurnValues,
            eBurnValueSource, eMergeAlgCurrent, pfnTransformer, pTransformArg,
            nPixels + iShape, nDataPixels + iShape);
        iShape++;
        delete poFeat;
      }
      VSIFree(padfAttrValues);

      // Only write image if not a single chunk is being rendered
      if (nYChunkSize < poDS->GetRasterYSize()) {
        eErr = poDS->RasterIO(GF_Write, 0, iY, poDS->GetRasterXSize(),
                              nThisYChunkSize, pabyChunkBuf,
                              poDS->GetRasterXSize(), nThisYChunkSize, eType,
                              nBandCount, panBandList, 0, 0, 0);
      }

      poLayer->ResetReading();

      if (!pfnProgress((iY + nThisYChunkSize) /
                           ((double)poDS->GetRasterYSize()),
                       "", pProgressArg)) {
        CPLError(CE_Failure, CPLE_UserInterrupt, "User terminated");
        eErr = CE_Failure;
      }
    }

    if (bNeedToFreeTransformer) {
      GDALDestroyTransformer(pTransformArg);
      pTransformArg = NULL;
      pfnTransformer = NULL;
    }

    delete[] nPixels;
    delete[] nDataPixels;
  }

  /* -------------------------------------------------------------------- */
  /*      Write out the image once for all layers if user requested       */
  /*      to render the whole raster in single chunk.                     */
  /* -------------------------------------------------------------------- */
  if (nYChunkSize == poDS->GetRasterYSize()) {
    poDS->RasterIO(GF_Write, 0, 0, poDS->GetRasterXSize(), nYChunkSize,
                   pabyChunkBuf, poDS->GetRasterXSize(), nYChunkSize, eType,
                   nBandCount, panBandList, 0, 0, 0);
  }

  /* -------------------------------------------------------------------- */
  /*      cleanup                                                         */
  /* -------------------------------------------------------------------- */
  VSIFree(pabyChunkBuf);

  return eErr;
}
