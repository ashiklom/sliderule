/*
 * Copyright (c) 2021, University of Washington
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the University of Washington nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF WASHINGTON AND CONTRIBUTORS
 * “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF WASHINGTON OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __vrt_raster__
#define __vrt_raster__

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "GeoRaster.h"
#include "GeoParms.h"

/******************************************************************************
 * VRT RASTER CLASS
 ******************************************************************************/

class VrtRaster: public GeoRaster
{
    public:

        /*--------------------------------------------------------------------
         * Methods
         *--------------------------------------------------------------------*/

        static void init    (void);
        static void deinit  (void);

    protected:

        /*--------------------------------------------------------------------
         * Methods
         *--------------------------------------------------------------------*/

                     VrtRaster          (lua_State* L, GeoParms* _parms, const char* vrt_file=NULL);
        void         openGeoIndex       (double lon=0, double lat=0) override;
        virtual void getIndexFile       (std::string& file, double lon=0, double lat=0);
        virtual bool getRasterDate      (raster_info_t& rinfo) = 0;
        bool         readGeoIndexData   (OGRPoint* point, int srcWindowSize, int srcOffset,
                                         void *data, int dstWindowSize, GDALRasterIOExtraArg *args) override;

        bool         findRasters        (OGRPoint &p) override;
        bool         findCachedRasters  (OGRPoint &p) override;
        void         buildVRT           (std::string& vrt_file, List<std::string>& rlist);

        /*--------------------------------------------------------------------
         * Data
         *--------------------------------------------------------------------*/
        std::string     vrtFile;

    private:

        /*--------------------------------------------------------------------
         * Data
         *--------------------------------------------------------------------*/
        GDALRasterBand *band;
        double          invGeot[6];
        uint32_t        groupId;
};

#endif  /* __vrt_raster__ */
