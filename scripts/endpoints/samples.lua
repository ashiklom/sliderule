--
-- ENDPOINT:    /source/samples
--
-- PURPOSE:     return a sample from the ArcticDEM given a lat and lon
--
-- INPUT:       rqst
--              {
--                  "samples": {<geoparms>}
--                  "coordinates": [
--                      [<longitude>, <latitude>],
--                      [<longitude>, <latitude>]...
--                  ]
--              }
--
-- OUTPUT:      samples
--

local json = require("json")

-- Request Parameters --
local rqst = json.decode(arg[1])
local coord = rqst["coordinates"]

-- Get Samples --
local dem = geo.raster(geo.parms(rqst[geo.PARMS]))

-- Build Table --
local samples = {}
if dem then
    for _, position in ipairs(coord) do
        local lon = position[1]
        local lat = position[2]
        local height = position[3] -- optional
        local sample, status = dem:sample(lon, lat, height)
        if status then
            table.insert(samples, sample)
        end
    end
end

-- Return Response
return json.encode({samples=samples})