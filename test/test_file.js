const LibltcNapiLimited = require("../lib/binding.js");
const assert = require("assert");

assert(LibltcNapiLimited, "The expected module is undefined");

function testBasic() {
    const decoder = new LibltcNapiLimited();
    console.log(decoder.decodeFile("timecode_foo.raw"));
}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");