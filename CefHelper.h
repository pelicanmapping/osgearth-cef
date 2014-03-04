#ifndef OSGEARTH_CEF_CEFHELPER_H
#define OSGEARTH_CEF_CEFHELPER_H

#include "BrowserClient.h"

#include <osgViewer/CompositeViewer>

namespace osgEarth { namespace Cef
{

class CefHelper
{
public:
    CefRefPtr<BrowserClient> load(osg::ArgumentParser& args, const std::string& htmlFile="");

    int run(osg::ArgumentParser& args, const std::string& htmlFile="");

    std::string usage() const;
};

} }

#endif
