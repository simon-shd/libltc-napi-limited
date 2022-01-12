const LibltcNapiLimited = require("../lib/binding.js");
const portAudio = require('naudiodon');
const assert = require("assert");

// To test streaming, I used Qlab to send LTC to an audio loopback device (BlackHole), then
// used this test to pull audio from the loopback device.
// Qlab: https://qlab.app/
// BlackHole: https://existential.audio/blackhole/

const DEVICE_ID = 8; // this is the address for the loopback device

assert(LibltcNapiLimited, "The expected module is undefined");

function testBasic() {
    const instance = new LibltcNapiLimited();

    var ai = new portAudio.AudioIO({
        inOptions: {
            channelCount: 1,
            sampleFormat: portAudio.SampleFormat8Bit,
            sampleRate: 44100,
            deviceId: DEVICE_ID,
            closeOnError: true // Close the stream if an audio error is detected, if set false then just log the error
        }
    });

    instance.setJumpFrames(4);

    instance.on('data', buf => {
        console.log(buf.toString());
        if (instance.getFps())
            console.log("fps: " + instance.getFps() + ((instance.isDropFrame()) ? " drop-frame" : " non-drop"));
    });

    ai.on('close', () => {
        console.log("closed");
    });

    ai.pipe(instance);
    ai.start();

}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an exception");