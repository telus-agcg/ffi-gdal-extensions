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

#ifndef GDAL_RASTERIZE_ORIGINAL_H_INCLUDED
#define GDAL_RASTERIZE_ORIGINAL_H_INCLUDED

#include "gdal_alg_priv.h"
#include "ogr_geometry.h"
#include <vector>

/******************************************************************************
 * Extracted from gdal_rasterize.cpp.  Unchanged except to remove static
 *keyword.
 * See original source at:
 * https://github.com/OSGeo/gdal/blob/tags/1.11.5/gdal/alg/llrasterize.cpp
 ****************************************************************************/

void GDALCollectRingsFromGeometry(OGRGeometry *poShape,
                                  std::vector<double> &aPointX,
                                  std::vector<double> &aPointY,
                                  std::vector<double> &aPointVariant,
                                  std::vector<int> &aPartSize,
                                  GDALBurnValueSrc eBurnValueSrc);

#endif /* ndef GDAL_RASTERIZE_ORIGINAL_H_INCLUDED */
