const addon = require('../build/Release/libltc-napi-limited-native');

function LibltcNapiLimited() {
    this.decodeFile = function(str) {
        return _addonInstance.decodeFile(str);
    };

    this.decodeStream = function(str) {
        return _addonInstance.decodeStream(str);
    };

    var _addonInstance = new addon.LibltcNapiLimited();
}

module.exports = LibltcNapiLimited;
