const addon = require('../build/Release/liblibltc-napi-limited-native');
const { Transform } = require('stream');

class LibltcNapiLimited extends Transform {

    constructor(options) {
        super(options);

        this.decodeFile = function(str) {
            return this._addonInstance.decodeFile(str);
        };

        this.decodeStream = function(str) {
            return this._addonInstance.decodeStream(str);
        };

        this.decodeChunk = function(chunk, cb) {
            return this._addonInstance.decodeChunk(chunk, cb);
        };

        // returns an int if fps has been determined, 0 if not
        this.getFps = () => {
            if (this._libltcData.determinedFps) {
                return this._libltcData.fps;
            } else {
                return 0;
            }
        };

        // returns a boolean if it's drop frame
        this.isDropFrame = () => {
            return this._libltcData.dropFrame;
        };

        this.setJumpFrames = (numFrames) => {
            this._libltcData.jumpFrames = +numFrames;
        };

        this._addonInstance = new addon.LibltcNapiLimited();
        this._libltcData = {
            dropFrame: false,
            previousFrame: null,
            determinedFps: false,
            fps: 0,
            jumpFrames: 0,
        };
    }

    _transform(chunk, encoding, callback) {
        function leftFillNum(num) {
            return num.toString().padStart(2, 0);
        }

        // adds 
        function jumpForward(tcStr, framesToJump, fps) {
            let match = tcStr.match(/(\d{2}):(\d{2}):(\d{2})([:|.])(\d{2})/);
            let hms = [+match[1], +match[2], +match[3]];
            let frames = +match[5] + framesToJump;

            if (frames > fps) {
                frames -= fps;
                hms[2]++;
                if (hms[2] >= 60) {
                    hms[2] -= 60;
                    hms[1]++;
                    if (hms[1] >= 60) {
                        hms[1] -= 60;
                        hms[0]++;
                    }
                }
            }
            tcStr = leftFillNum(hms[0]) + ':' +
                leftFillNum(hms[1]) + ':' +
                leftFillNum(hms[2]) + match[4] +
                leftFillNum(frames);
            return tcStr;
        }

        let frameNum;
        let return_opt = this.decodeChunk(chunk, tc => {
            frameNum = +tc.slice(-2);
            if (frameNum === 0 && this._libltcData.previousFrame !== null) {
                if (this._libltcData.previousFrame > 22) {
                    this._libltcData.fps = this._libltcData.previousFrame + 1;
                    this._libltcData.determinedFps = true;
                }
            } else {
                this._libltcData.previousFrame = frameNum;
            }

            if (this._libltcData.jumpFrames !== 0 && this._libltcData.determinedFps) {
                tc = jumpForward(tc, this._libltcData.jumpFrames, this._libltcData.fps-1);
            }
            this.push(tc);
        });
        this._libltcData.dropFrame = (return_opt.dropFrame === 1);
        callback();
    }

}

module.exports = LibltcNapiLimited;