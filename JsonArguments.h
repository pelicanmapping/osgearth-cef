#ifndef OSGEARTH_CEF_JSONARGUMENTS_H
#define OSGEARTH_CEF_JSONARGUMENTS_H

#include <osgEarth/JsonUtils>

namespace osgEarth { namespace Cef
{

class JsonArguments
{
public:
    JsonArguments( const std::string& json_string );
    std::string operator [] ( const std::string& key ) const;

private:
    osgEarth::Json::Value _obj;
};

} }

#endif