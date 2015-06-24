#include "GDALLayer"

#include <osg/Notify>

#include <gdal_priv.h>
#include <gdalwarper.h>
#include <ogr_spatialref.h>

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

        // The band count
        retval->SetValue("bands", CefV8Value::CreateInt(ds->GetRasterCount()), V8_PROPERTY_ATTRIBUTE_NONE);

        // Close the dataset.
        GDALClose( ds );
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
