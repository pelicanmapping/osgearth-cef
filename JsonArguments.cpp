#include "JsonArguments"

using namespace osgEarth::Cef;


JsonArguments::JsonArguments( const std::string& json_string )
{
    osgEarth::Json::Reader reader;
    if ( !reader.parse( json_string, _obj ) )
    {
        OE_WARN << "JSON PARSING ERROR in: " << json_string << std::endl;
    }
    else
    {
        OE_DEBUG << "Parsed: " << json_string << std::endl;
    }
}

std::string JsonArguments::operator [] ( const std::string& key ) const
{
    osgEarth::Json::Value value = _obj.get( key, osgEarth::Json::Value() );

    if ( value.isNull() )
    {
        return "";
    }
    else if ( value.isConvertibleTo(osgEarth::Json::stringValue) )
    {
        return value.asString();
    }
    else
    {
        osgEarth::Json::FastWriter writer;
        return writer.write( value );
    }
}