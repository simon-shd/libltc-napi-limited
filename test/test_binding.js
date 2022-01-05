const LibltcNapiLimited = require("../lib/binding.js");
const portAudio = require('naudiodon');

const assert = require("assert");

assert(LibltcNapiLimited, "The expected module is undefined");

function testBasic() {
    const instance = new LibltcNapiLimited();

    var ai = new portAudio.AudioIO({
        inOptions: {
            channelCount: 1,
            sampleFormat: portAudio.SampleFormat16Bit,
            sampleRate: 44100,
            deviceId: 1,
            closeOnError: true // Close the stream if an audio error is detected, if set false then just log the error
        }
    });

    let result_queue, i;
    ai.on('data', buf => {
        result_queue = instance.decodeStream(buf);
        // console.log('done processing');
        // console.dir(result_queue);
        for (i=0; i<result_queue.length; i++)
            console.log(result_queue[i]);
    });

    ai.start();
    // console.log(portAudio.getDevices());

    // assert(instance.decodeStream, "The expected method is not defined");
    // assert.strictEqual(instance.decodeStream("rawAudio.raw"), "whee", "Unexpected value returned");
}
/* 
function testInvalidParams()
{
    const instance = new LibltcNapiLimited();
}
 */
assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");
// assert.throws(testInvalidParams, undefined, "testInvalidParams didn't throw");

console.log("Tests passed- everything looks OK!");