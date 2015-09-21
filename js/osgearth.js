/* *****************************
 * oeutil
 * *****************************/

(function() {

    // create global oeutil object
    this.oeutil = function() {
    }

    this.oeutil.extend = function(target, source) {
        target = target || {};
        for (var prop in source) {
            if (typeof source[prop] === 'object') {
                target[prop] = extend(target[prop], source[prop]);
            } else {
                target[prop] = source[prop];
            }
        }
        return target;
    }

})();



/* *****************************
 * osgearth
 * *****************************/

(function() {

    // private function
    function doQuery(request, options) {
        var query = {
            request: JSON.stringify(request),
            persistent: false,
            onSuccess: function(response) { },
            onFailure: function(error_code, error_message) { alert("Query error(" + error_code + "): " + error_message); }
        };
        oeutil.extend(query, options);

        return window.cefQuery(query);
    }

    if (!osgearth) {
        osgearth = {};
    }

    this.osgearth.createMap = function(id, x, y, width, height, earthfile, options) {
        this.execute("_OE_create_map", { id: id, x: x, y: y, width: width, height: height, file: earthfile }, options);
    }


    this.osgearth.setMap = function(id, x, y, width, height, options) {
        this.execute("_OE_set_map", { id: id, x: x, y: y, width: width, height: height }, options);
    }


    this.osgearth.execute = function(command, args, options) {
        var request = oeutil.extend({command: command}, args);
        return doQuery(request, options);
    }


    this.osgearth.openFileDialog = function(options, multiple, extensions) {
        if (multiple === true) {
            this.execute("_OE_open_multifile_dialog", { filters: extensions }, options);
        }
        else {
            this.execute("_OE_open_file_dialog", { filters: extensions }, options);
        }
    }

    this.osgearth.openFolderDialog = function(options) {
        this.execute("_OE_open_folder_dialog", { }, options);
    }

})();


/* *****************************
 * Map
 * *****************************/
 
function Map(id, x, y, width, height, earthfile, options) {
    this._id = id;
    osgearth.createMap(id, x, y, width, height, earthfile, options);
}
 
Map.prototype._id;

Map.prototype.loadFile = function(earthfile, options) {
    osgearth.execute("_OE_map_load_file", { id: this._id, url: earthfile }, options);
}

Map.prototype.resize = function(x, y, width, height, options) {
    osgearth.setMap(this._id, x, y, width, height, options);
}
 
Map.prototype.home = function(options) {
    osgearth.execute("_OE_map_home", { id: this._id }, options);
}

Map.prototype.getImageLayers = function(options) {
    osgearth.execute("_OE_get_image_layers", { id: this._id }, options);
}

Map.prototype.getElevationLayers = function(options) {
    osgearth.execute("_OE_get_elevation_layers", { id: this._id }, options);
}

Map.prototype.getModelLayers = function(options) {
    osgearth.execute("_OE_get_model_layers", { id: this._id }, options);
}

Map.prototype.addModelListener = function(listener) {
    osgearth.execute("_OE_add_map_listener", { id: this._id }, { persistent: true, onSuccess: function(response) { listener(response); } });
}
