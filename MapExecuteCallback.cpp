#include "MapExecuteCallback"

#include <osgEarthUtil/EarthManipulator>


using namespace osgEarth::Cef;


#define LC "[MapExecuteCallback] "


ExecuteCallback::ReturnVal* MapExecuteCallback::execute( int64 query_id, const std::string& command, const JsonArguments &args, CefRefPtr<CefMessageRouterBrowserSide::Callback> persistentCallback )
{
    if (command == "_OE_create_map")
    {
        std::string id = args["id"];
        if (id.empty())
            return new ReturnVal("Error creating map: no id specified.", -1);

        if (_client->_mapViews.find(id) != _client->_mapViews.end())
            return new ReturnVal("Error creating map: duplicate id specified.", -1);

        int x = osgEarth::as<int>(args["x"], 0);
        int y = osgEarth::as<int>(args["y"], 0);
        int width = osgEarth::as<int>(args["width"], 0);
        int height = osgEarth::as<int>(args["height"], 0);

        if (width <= 0 || height <= 0)
            return new ReturnVal("Error creating map: width and height must be > 0.", -1);

        OE_DEBUG << LC << "createMap(" << id << "): at " << x << ", " << y <<  "  size " << width << " x " << height << std::endl;

        osgViewer::View* mapView = new osgViewer::View();
        mapView->getCamera()->setGraphicsContext(_client->_viewer->getView(0)->getCamera()->getGraphicsContext());
        mapView->getCamera()->setNearFarRatio(0.00002);
        mapView->getCamera()->setViewport( x, y, width, height);
        mapView->getCamera()->setRenderOrder(osg::Camera::PRE_RENDER);
        mapView->setCameraManipulator( new osgEarth::Util::EarthManipulator() );    
        mapView->getCamera()->setProjectionMatrixAsPerspective(30.0, double(width) / double(height), 1.0, 1000.0);

        _client->_mapViews[id] = mapView;

        _client->_viewer->addView( mapView );

        std::string mapFile = args["file"];
        if (!mapFile.empty())
        {
            osg::Node* node = osgDB::readNodeFile(mapFile);
            if (node)
            {
                mapView->setSceneData(node);
                _client->_mapNodes[id] = osgEarth::MapNode::findMapNode(node);
            }
            else
            {
                _client->_mapNodes[id] = 0L;
                OE_WARN << LC << "Could not load map file: " << mapFile << std::endl;
            }
        }
        else
        {
            _client->_mapNodes[id] = 0L;
        }

        return new ReturnVal();
    }
    else if (command == "_OE_set_map")
    {
        std::string id = args["id"];
        if (id.empty())
            return new ReturnVal("Error setting map: no id specified.", -1);

        osg::ref_ptr<osgViewer::View> mapView = _client->getMapView(id);
        if (!mapView.valid())
            return new ReturnVal("Error setting map: specified id \"" + id + "\" not found.", -1);

        int x = osgEarth::as<int>(args["x"], 0);
        int y = osgEarth::as<int>(args["y"], 0);
        int width = osgEarth::as<int>(args["width"], 0);
        int height = osgEarth::as<int>(args["height"], 0);

        if (width <= 0 || height <= 0)
            return new ReturnVal("Error setting map: width and height must be > 0.", -1);

        OE_DEBUG << LC << "setMap(" << id << "): at " << x << ", " << y <<  "  size " << width << " x " << height << std::endl;

        mapView->getCamera()->setViewport( x, y, width, height);
        mapView->getCamera()->setProjectionMatrixAsPerspective(30.0, double(width) / double(height), 1.0, 1000.0);

        return new ReturnVal();
    }
    else if (command == "_OE_map_home")
    {
        std::string id = args["id"];
        if (id.empty())
            return new ReturnVal("Map error: no id specified.", -1);

        osg::ref_ptr<osgViewer::View> mapView = _client->getMapView(id);
        if (!mapView.valid())
            return new ReturnVal("Map error: id not found.", -1);

        mapView->getCameraManipulator()->home(0.0);

        return new ReturnVal();
    }
    else if (command == "_OE_get_image_layers")
    {
        std::string id = args["id"];
        if (id.empty())
            return new ReturnVal("Map error: no id specified.", -1);

        osg::ref_ptr<osgEarth::MapNode> mapNode = _client->getMapNode(id);
        if (!mapNode.valid())
            return new ReturnVal("Map error: id not found.", -1);

        osgEarth::ImageLayerVector layers;
        mapNode->getMap()->getImageLayers(layers);

        std::stringstream output;
        output << "{\"layers\": [";
        for (osgEarth::ImageLayerVector::const_iterator it = layers.begin(); it != layers.end(); ++it)
        {
            if (it != layers.begin())
                output << ",";

            output << "{\"id\": " << (*it)->getUID() << ", \"name\": \"" << (*it)->getName() << "\"}";
        }
        output << "]}";

        return new ReturnVal(output.str());
    }
    else if (command == "_OE_get_elevation_layers")
    {
        std::string id = args["id"];
        if (id.empty())
            return new ReturnVal("Map error: no id specified.", -1);

        osg::ref_ptr<osgEarth::MapNode> mapNode = _client->getMapNode(id);
        if (!mapNode.valid())
            return new ReturnVal("Map error: id not found.", -1);

        osgEarth::ElevationLayerVector layers;
        mapNode->getMap()->getElevationLayers(layers);

        std::stringstream output;
        output << "{\"layers\": [";
        for (osgEarth::ElevationLayerVector::const_iterator it = layers.begin(); it != layers.end(); ++it)
        {
            if (it != layers.begin())
                output << ",";

            output << "{\"id\": " << (*it)->getUID() << ", \"name\": \"" << (*it)->getName() << "\"}";
        }
        output << "]}";

        return new ReturnVal(output.str());
    }
    else if (command == "_OE_get_model_layers")
    {
        std::string id = args["id"];
        if (id.empty())
            return new ReturnVal("Map error: no id specified.", -1);

        osg::ref_ptr<osgEarth::MapNode> mapNode = _client->getMapNode(id);
        if (!mapNode.valid())
            return new ReturnVal("Map error: id not found.", -1);

        osgEarth::ModelLayerVector layers;
        mapNode->getMap()->getModelLayers(layers);

        std::stringstream output;
        output << "{\"layers\": [";
        for (osgEarth::ModelLayerVector::const_iterator it = layers.begin(); it != layers.end(); ++it)
        {
            if (it != layers.begin())
                output << ",";

            output << "{\"id\": " << (*it)->getUID() << ", \"name\": \"" << (*it)->getName() << "\"}";
        }
        output << "]}";

        return new ReturnVal(output.str());
    }
    else if (command == "_OE_add_map_listener")
    {
        std::string id = args["id"];
        if (id.empty())
            return new ReturnVal("Map error: no id specified.", -1);

        osg::ref_ptr<osgEarth::MapNode> mapNode = _client->getMapNode(id);
        if (!mapNode.valid())
            return new ReturnVal("Map error: id not found.", -1);

        if (!persistentCallback)
            return new ReturnVal("Map listener callback not persistent.", -1);

        osg::ref_ptr<MapQueryCallback> cb = new MapQueryCallback(id, persistentCallback);
        mapNode->getMap()->addMapCallback(cb);
        _mapCallbacks[query_id] = cb;
        
        return new ReturnVal();
    }

    return 0L;
}

void MapExecuteCallback::MapQueryCallback::onMapModelChanged( const osgEarth::MapModelChange& change )
{
    std::string eventName = "";
    osgEarth::Json::Value eventData;

    switch( change.getAction() )
    {
    case osgEarth::MapModelChange::ADD_ELEVATION_LAYER: 
        eventName = "elevationlayeradded";
        eventData["id"] = change.getLayer()->getUID();
        eventData["index"] = change.getFirstIndex();
        break;
    case osgEarth::MapModelChange::ADD_IMAGE_LAYER:
        eventName = "imagelayeradded";
        eventData["id"] = change.getLayer()->getUID();
        eventData["index"] = change.getFirstIndex();
        break;
    case osgEarth::MapModelChange::ADD_MASK_LAYER:
        eventName = "masklayeradded";
        eventData["id"] = change.getMaskLayer()->getUID();
    case osgEarth::MapModelChange::ADD_MODEL_LAYER:
        eventName = "modellayeradded";
        eventData["id"] = change.getModelLayer()->getUID();
        break;
    case osgEarth::MapModelChange::REMOVE_ELEVATION_LAYER:
        eventName = "elevationlayerremoved";
        eventData["id"] = change.getLayer()->getUID();
        eventData["index"] = change.getFirstIndex();
        break;
    case osgEarth::MapModelChange::REMOVE_IMAGE_LAYER:
        eventName = "imagelayerremoved";
        eventData["id"] = change.getLayer()->getUID();
        eventData["index"] = change.getFirstIndex();
        break;
    case osgEarth::MapModelChange::REMOVE_MASK_LAYER:
        eventName = "masklayerremoved";
        eventData["id"] = change.getMaskLayer()->getUID();
        break;
    case osgEarth::MapModelChange::REMOVE_MODEL_LAYER:
        eventName = "modellayerremoved";
        eventData["id"] = change.getModelLayer()->getUID();
        break;
        onModelLayerRemoved( change.getModelLayer() ); break;
    case osgEarth::MapModelChange::MOVE_ELEVATION_LAYER:
        eventName = "elevationlayermoved";
        eventData["id"] = change.getLayer()->getUID();
        eventData["old_index"] = change.getFirstIndex();
        eventData["new_index"] = change.getSecondIndex();
        break;
    case osgEarth::MapModelChange::MOVE_IMAGE_LAYER:
        eventName = "imagelayermoved";
        eventData["id"] = change.getLayer()->getUID();
        eventData["old_index"] = change.getFirstIndex();
        eventData["new_index"] = change.getSecondIndex();
        break;
    case osgEarth::MapModelChange::MOVE_MODEL_LAYER:
        eventName = "modellayermoved";
        eventData["id"] = change.getModelLayer()->getUID();
        eventData["old_index"] = change.getFirstIndex();
        eventData["new_index"] = change.getSecondIndex();
        break;
    }

    if (eventName.length() > 0)
    {
        eventData["eventName"] = eventName;
        eventData["mapId"] = mapId;
        osgEarth::Json::FastWriter writer;
        std::string data = writer.write(eventData);

        callback->Success(data);
    }
}