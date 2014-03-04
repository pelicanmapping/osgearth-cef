#ifndef OSGEARTH_CEF_EXECUTECALLBACK_H
#define OSGEARTH_CEF_EXECUTECALLBACK_H


#include "JsonArguments.h"

#include "include/wrapper/cef_message_router.h"


namespace osgEarth { namespace Cef
{

class ExecuteCallback : public osg::Referenced
{
public:

    struct ReturnVal : public osg::Referenced
    {
        ReturnVal() : value(""), errorCode(0) { }
        ReturnVal(const std::string& val, int errCode=0) : value(val), errorCode(errCode) { }

        std::string value;
        int errorCode;
    };

    // called when a command is received from the javascript object
    virtual ReturnVal* execute(int64 query_id, const std::string& command, const JsonArguments &args, CefRefPtr<CefMessageRouterBrowserSide::Callback> persistentCallback) { return 0L; }

    // called when a command is canceled
    virtual bool cancel(int64 query_id) { return false; }
};

} }

#endif