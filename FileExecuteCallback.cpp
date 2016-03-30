#include "FileExecuteCallback"
#include "ExecuteCallback"
#include "BrowserClient"

using namespace osgEarth::Cef;

#define LC "[FileExecuteCallback] "

namespace
{
    class LocalFileDialogCallback : public CefRunFileDialogCallback  
    {
    public:
        LocalFileDialogCallback(CefRefPtr<CefMessageRouterBrowserSide::Callback> callback) : _browserCallback(callback) { }

        void OnFileDialogDismissed(int selected_accept_filter, const std::vector<CefString>& file_paths)
        {
            if (_browserCallback)
            {
                if (file_paths.size() == 0)
                {
                    _browserCallback->Failure(-1, "");
                }
                else
                {
                    std::string s = "";
                    for (int i=0; i < file_paths.size(); i++)
                        s.append(file_paths[i].ToString() + (i < file_paths.size() - 1 ? "," : ""));

                    _browserCallback->Success(s);
                }
            }
        }

    protected:

        CefRefPtr<CefMessageRouterBrowserSide::Callback> _browserCallback;

        IMPLEMENT_REFCOUNTING(LocalFileDialogCallback);
        DISALLOW_COPY_AND_ASSIGN(LocalFileDialogCallback);
    };
}


ExecuteCallback::ReturnVal* FileExecuteCallback::execute( int64 query_id, const std::string& command, const JsonArguments &args, bool persistent, CefRefPtr<CefMessageRouterBrowserSide::Callback> callback )
{
    if (command == "_OE_open_file_dialog" || command == "_OE_open_multifile_dialog" || command == "_OE_open_folder_dialog")
    {
        bool multiple = command == "_OE_open_multifile_dialog";
        bool folder = command == "_OE_open_folder_dialog";

        CefString path = args["path"];

        std::vector<CefString> filters;
        std::string filterString = args["filters"];

        while (filterString.length() > 0)
        {
          std::string::size_type pos = filterString.find_first_of(", ");
          if (pos == std::string::npos)
          {
              filters.push_back(filterString);
              filterString = "";
          }
          else
          {
              if (pos > 0)
              {
                filters.push_back(filterString.substr(0, pos));
              }

              filterString = filterString.substr(pos + 1);
          }
        }

        _client->getBrowser()->GetHost()->RunFileDialog(
            (multiple ? CefBrowserHost::FileDialogMode::FILE_DIALOG_OPEN_MULTIPLE : folder ? CefBrowserHost::FileDialogMode::FILE_DIALOG_OPEN_FOLDER : CefBrowserHost::FileDialogMode::FILE_DIALOG_OPEN),
            "", // title
            path, // default_file_path
            filters,  // accept_filters
            0,  // selected_accept_filter
            new LocalFileDialogCallback(callback));

        return new ReturnVal(true);
    }
    else if (command == "_OE_save_file_dialog")
    {
        CefString path = args["path"];

        std::vector<CefString> filters;
        std::string filterString = args["filters"];

        while (filterString.length() > 0)
        {
          std::string::size_type pos = filterString.find_first_of(", ");
          if (pos == std::string::npos)
          {
              filters.push_back(filterString);
              filterString = "";
          }
          else
          {
              if (pos > 0)
              {
                filters.push_back(filterString.substr(0, pos));
              }

              filterString = filterString.substr(pos + 1);
          }
        }

        _client->getBrowser()->GetHost()->RunFileDialog(
            CefBrowserHost::FileDialogMode::FILE_DIALOG_SAVE,
            "", // title
            path, // default_file_path
            filters,  // accept_filters
            0,  // selected_accept_filter
            new LocalFileDialogCallback(callback));

        return new ReturnVal(true);
    }

    return 0L;
}

