#include "CefHelper"
#include "ExecuteCallback"

#include <osg/Notify>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgDB/WriteFile>

#include "include/cef_app.h"

#include "OECefApp"
#include "BrowserClient"
#include "GDALResourceHandler"
#include <osgEarth/ThreadingUtils>

using namespace osgEarth::Cef;


#define LC "[CefHelper] "

CefRefPtr<BrowserClient> CefHelper::load(osg::ArgumentParser& args, const std::string& htmlFile)
{
    // Initialize CEF
    CefMainArgs cef_args;
    CefRefPtr<CefApp> cef_app = new OECefApp();

    int exitCode = CefExecuteProcess(cef_args, cef_app, 0L);
    if (exitCode >= 0)
    {
        return 0L;
    }

    {
        CefSettings settings;

        if (getenv("CEF_RESOURCES_DIR") != 0)
            CefString(&settings.resources_dir_path) = getenv("CEF_RESOURCES_DIR");
        
        if (getenv("CEF_LOCALES_DIR") != 0)
            CefString(&settings.locales_dir_path) = getenv("CEF_LOCALES_DIR");

        settings.windowless_rendering_enabled = true;

        bool result = CefInitialize(cef_args, settings, cef_app, 0L);
        if (!result)
        {
            OE_WARN << LC << "CefInitialize failed." << std::endl;
            return 0L;
        }

        CefRegisterSchemeHandlerFactory("gdal", "gdal", new GDALHandlerFactory());
    }


    // Read in the html file if needed
    std::string url = "";
    if (htmlFile.length() > 0)
    {
      url = htmlFile;
    }
    else
    {
        if (!args.read("--url", url))
        {
            for( int i=0; i<args.argc(); ++i )
            {
                if ( osgDB::getLowerCaseFileExtension(args[i]) == "html" )
                {
                    url = args[i];
                    args.remove(i);
                    break;
                }
            }
        }
    }
    // No file specified, try the default index.html
    if (url.length() == 0)
        url = osgDB::getRealPath("index.html");

    // No file specified and index.html does not exist so exit
    if (url.length() == 0)
    {
        OE_WARN << LC << "No html file specified, exiting..." << std::endl;
        return 0L;
    }

    // A file or url was specified so get the full address
    std::string fullPath = osgDB::containsServerAddress(url) ? url : osgDB::getRealPath(url);


    // Setup a CompositeViewer
    osg::ref_ptr<osgViewer::CompositeViewer> viewer = new osgViewer::CompositeViewer(args);
    viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);

    // prevents "ESC" from killing the application
    viewer->setKeyEventSetsDone( 0 );
    viewer->setQuitEventSetsDone( false );
    

    // Create the BrowserClient
    CefRefPtr<BrowserClient> browserClient = new BrowserClient(viewer.get(), fullPath, 1440, 800);


    return browserClient;
}

int CefHelper::run(osg::ArgumentParser& args, const std::string& htmlFile)
{
    // Call load to initialize CEF and create the BrowserClient
    CefRefPtr<BrowserClient> browserClient = load(args, htmlFile);

    // Start main loop
    if (browserClient)
    {
        while (!browserClient->getViewer()->done())
        {
            {
                // We lock the BrowserClient mutex here to protect access to shard resources like
                // the internal osg::Image for the browser canvas
                OpenThreads::ScopedLock< OpenThreads::Mutex > lk(browserClient->getMutex());
                browserClient->getViewer()->frame();
            }
            CefDoMessageLoopWork();
        }
    }

    // Shutdown the CEF processes
    CefShutdown();

    return 0; 
}

