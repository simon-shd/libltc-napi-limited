const LibltcNapiLimited = require("../lib/binding.js");
const portAudio = require('naudiodon');
const assert = require("assert");

// To test streaming, I used Qlab to send LTC to an audio loopback device (BlackHole), then
// used this test to pull audio from the loopback device.
// Qlab: https://qlab.app/
// BlackHole: https://existential.audio/blackhole/

const DEVICE_ID = 1; // this is the address for the loopback device

assert(LibltcNapiLimited, "The expected module is undefined");

function testBasic() {
    const instance = new LibltcNapiLimited();

    var ai = new portAudio.AudioIO({
        inOptions: {
            channelCount: 1,
            sampleFormat: portAudio.SampleFormat16Bit,
            sampleRate: 44100,
            deviceId: DEVICE_ID,
            closeOnError: true // Close the stream if an audio error is detected, if set false then just log the error
        }
    });

    let result_queue, i;
    ai.on('data', buf => {
        console.dir(buf[0]);
        result_queue = instance.decodeStream(buf);
        
        for (i=0; i<result_queue.length; i++)
            console.log(result_queue[i]);
    });

    ai.start();
}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");