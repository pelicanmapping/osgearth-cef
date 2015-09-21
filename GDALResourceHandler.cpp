#include "GDALResourceHandler"
#include <osgDB/ReadFile>

using namespace osgEarth::Cef;

GDALResourceHandler::GDALResourceHandler():
_readOffset(0)
{
}

bool GDALResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback)
{
    // Evaluate |request| to determine proper handling...
    // Execute |callback| once header information is available.
    // Return true to handle the request.
    std::string filename = request->GetURL().ToString().substr(12);
    _image = osgDB::readImageFile(filename + ".gdal");

    osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("jpg");
    std::stringstream out;
    rw->writeImage(*_image.get(), out);
    _data = out.str();

    // Call the callback Contiue method, this should be called from a background thread once the image is actually loaded.
    callback->Continue();
    return true;
}

void GDALResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
    int64& response_length,
    CefString& redirectUrl)
{
    // Populate the response headers.
    response->SetMimeType("image/jpeg");
    response->SetStatus(200);

    // Specify the resulting response length.
    response_length = _data.size();
}

void GDALResourceHandler::Cancel(){
    // Cancel the response...
}

bool GDALResourceHandler::ReadResponse(void* data_out,
    int bytes_to_read,
    int& bytes_read,
    CefRefPtr<CefCallback> callback)
{
    // Read up to |bytes_to_read| data into |data_out| and set |bytes_read|.
    // If data isn't immediately available set bytes_read=0 and execute
    // |callback| asynchronously.
    // Return true to continue the request or false to complete the request.
    bool hasData = false;
    int dataLength = _data.size();
    unsigned int bytes_remaining = dataLength - _readOffset;
    if (_readOffset < dataLength)
    {
        unsigned int numToRead = min(bytes_to_read, bytes_remaining);
        memcpy(data_out,
            _data.c_str() + _readOffset,
            numToRead);
        bytes_read = numToRead;
        _readOffset += numToRead;
        hasData = true;
    }
    else
    {
        bytes_read = 0;
    }

    return hasData;
}

/*************************************************************/

CefRefPtr<CefResourceHandler> GDALHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request)
{
    // Return a new resource handler instance to handle the request.
    return new GDALResourceHandler();
}