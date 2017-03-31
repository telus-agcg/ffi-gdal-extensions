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

/******************************************************************************
 * Extracted from gdalrasterize.cpp.  Unchanged except to remove static keyword.
 * See original source at:
 * https://github.com/OSGeo/gdal/blob/tags/1.11.5/gdal/alg/gdalrasterize.cpp
 ****************************************************************************/

#include "gdal_rasterize.h"
#include "gdal.h"
#include "gdal_alg_priv.h"
#include "ogr_api.h"
#include "ogr_geometry.h"
#include "ogr_spatialref.h"
#include "ogrsf_frmts.h"
#include <vector>

/************************************************************************/
/*                    GDALCollectRingsFromGeometry()                    */
/*      This one is identical to the GDAL source.  Had to copy it since */
/*      it was not available from the headers.                          */
/************************************************************************/
void GDALCollectRingsFromGeometry(OGRGeometry *poShape,
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
