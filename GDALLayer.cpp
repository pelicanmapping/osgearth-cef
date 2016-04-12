#include "GDALLayer"

#include <osg/Notify>

#include <gdal_priv.h>
#include <gdalwarper.h>
#include <ogr_spatialref.h>

#include <osgEarth/SpatialReference>
#include <osgEarth/GeoData>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarth/ImageLayer>

using namespace osgEarth::Cef;


class GDALUserData : public CefBase
{
public:
    GDALUserData(GDALDataset* ds)
        : _ds(ds)
    {
    }

    GDALDataset* _ds;

    IMPLEMENT_REFCOUNTING(GDALUserData);
};




GDALV8Handler::GDALV8Handler()
{
}

bool GDALV8Handler::Execute(const CefString& name,
    CefRefPtr<CefV8Value> object,
    const CefV8ValueList& arguments,
    CefRefPtr<CefV8Value>& retval,
    CefString& exception)
{
    if (name == "GDALOpen")
    {
        // Open the dataset.
        std::string filename = arguments[0]->GetStringValue().ToString();
        GDALDataset* ds = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly );
        if (!ds)
        {
            // If we can't open the dataset, just return NULL.
            retval = CefV8Value::CreateNull();
            return true;
        }

        // Create an object and populate it with info about the dataset.
        retval = CefV8Value::CreateObject(0);

        // The size of the image
        CefRefPtr< CefV8Value> size = CefV8Value::CreateArray(2);
        size->SetValue(0,  CefV8Value::CreateInt(ds->GetRasterXSize() ) );
        size->SetValue(1,  CefV8Value::CreateInt(ds->GetRasterYSize() ) );
        retval->SetValue("size", size, V8_PROPERTY_ATTRIBUTE_NONE);

        // The SRS
        retval->SetValue("projection", CefV8Value::CreateString(ds->GetProjectionRef()), V8_PROPERTY_ATTRIBUTE_NONE );

        // The GeoTransform info
        //double        adfGeoTransform[6];
        //if( ds->GetGeoTransform( adfGeoTransform ) == CE_None )
        //{
        //    CefRefPtr< CefV8Value> origin = CefV8Value::CreateArray(2);
        //    origin->SetValue(0,  CefV8Value::CreateDouble(adfGeoTransform[0] ) );
        //    origin->SetValue(1,  CefV8Value::CreateDouble(adfGeoTransform[3] ) );
        //    retval->SetValue("origin", origin, V8_PROPERTY_ATTRIBUTE_NONE);

        //    CefRefPtr< CefV8Value> pixel = CefV8Value::CreateArray(2);
        //    pixel->SetValue(0,  CefV8Value::CreateDouble(adfGeoTransform[1] ) );
        //    pixel->SetValue(1,  CefV8Value::CreateDouble(adfGeoTransform[5] ) );
        //    retval->SetValue("pixelsize", pixel, V8_PROPERTY_ATTRIBUTE_NONE);
        //}

        // The lat/lon bounds
        double adfGeoTransform[6];
        if( ds->GetGeoTransform( adfGeoTransform ) == CE_None )
        {
            double width = ds->GetRasterXSize();
            double height = ds->GetRasterYSize();
            double minX = adfGeoTransform[0];
            double minY = adfGeoTransform[3] + width * adfGeoTransform[4] + height * adfGeoTransform[5];
            double maxX = adfGeoTransform[0] + width * adfGeoTransform[1] + height * adfGeoTransform[2];
            double maxY = adfGeoTransform[3];

            const osgEarth::SpatialReference* srsIn = osgEarth::SpatialReference::get(ds->GetProjectionRef());
            const SpatialReference* srsOut = SpatialReference::get("epsg:4326");

            osgEarth::GeoPoint minIn(srsIn, minX, minY);
            osgEarth::GeoPoint maxIn(srsIn, maxX, maxY);

            osgEarth::GeoPoint minOut = minIn.transform(srsOut);
            osgEarth::GeoPoint maxOut = maxIn.transform(srsOut);

            retval->SetValue("minLat", CefV8Value::CreateDouble(minOut.y()), V8_PROPERTY_ATTRIBUTE_NONE);
            retval->SetValue("minLon", CefV8Value::CreateDouble(minOut.x()), V8_PROPERTY_ATTRIBUTE_NONE);
            retval->SetValue("maxLat", CefV8Value::CreateDouble(maxOut.y()), V8_PROPERTY_ATTRIBUTE_NONE);
            retval->SetValue("maxLon", CefV8Value::CreateDouble(maxOut.x()), V8_PROPERTY_ATTRIBUTE_NONE);
        }

        // The band count
        retval->SetValue("bands", CefV8Value::CreateInt(ds->GetRasterCount()), V8_PROPERTY_ATTRIBUTE_NONE);

        // Close the dataset.
        GDALClose( ds );

        // Use the TileSource to determine the max lod level
        osgEarth::Drivers::GDALOptions fileOpt;
        fileOpt.url() = filename;
        osg::ref_ptr<osgEarth::ImageLayer> imageLayer = new ImageLayer( ImageLayerOptions("image", fileOpt) );

        unsigned int maxLevel = 0;
        for (osgEarth::DataExtentList::iterator it = imageLayer->getTileSource()->getDataExtents().begin(); it != imageLayer->getTileSource()->getDataExtents().end(); ++it)
        {
          if (it->maxLevel().isSet() && it->maxLevel().value() > maxLevel)
          {
              maxLevel = it->maxLevel().value();
          }
        }

        retval->SetValue("maxLevel", CefV8Value::CreateInt(maxLevel), V8_PROPERTY_ATTRIBUTE_NONE);

        return true;
    }
    return false;
}

namespace
{
    std::string code =
        "var gdal = {};"
        "(function() {"
        "    gdal.open = function(filename) {"
        "        native function GDALOpen();"
        "        return GDALOpen(filename);"
        "    };"
        "})();";
}


void GDALAPI::AddGDALExtensions(CefRefPtr<CefV8Value> global)
{
    // set up GDAL and OGR.
    OGRRegisterAll();
    GDALAllRegister();

    CefRefPtr< GDALV8Handler > handler = new GDALV8Handler();
    CefRegisterExtension("gdal", code, handler);
}
