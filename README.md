[![code style: prettier](https://img.shields.io/badge/code_style-prettier-ff69b4.svg?style=flat-square)](https://github.com/prettier/prettier)


# <a name="project-name"></a>Libltc NAPI (Limited)

> libltc-napi-limited is a limited (decode-only) implementation of the libltc C library in NodeJS, using node-addon-api. It can decode a raw audio file or an audio stream.

(from the Libltc website):

Linear (or Longitudinal) Timecode (LTC) is an encoding of timecode data as a Manchester-Biphase encoded 
audio signal. The audio signal is commonly recorded on a VTR track or other storage media. 

The libltc C library provides functionality to encode and decode LTC from/to timecode, including SMPTE date support.

**This module is experimental and somewhat unstable - it should not be used in a production environment without doing your own due diligence on whether it will work for you**

## Prerequisites

This project requires NodeJS (version 8 or later) and NPM.
[Node](http://nodejs.org/) and [NPM](https://npmjs.org/) are really easy to install.
To make sure you have them available on your machine,
try running the following command.

```sh
$ npm -v && node -v
6.4.1
v8.16.0
```
Since the project makes use of a C/C++ library, you also need tools to compile C code. See the [Node-API Resources](https://nodejs.github.io/node-addon-examples/getting-started/tools) help files and tutorials for the prerequisites for your system, and for help installing them.

This project has been tested on MacOS (12.1) and Windows 10.

## Table of contents

- [LibLTC NAPI (Limited)](#project-name)
  - [Prerequisites](#prerequisites)
  - [Table of contents](#table-of-contents)
  - [Getting Started](#getting-started)
  - [Installation](#installation)
  - [Usage](#usage)
    - [decodeStream](#decodeStream)
    - [decodeFile](#decodeFile)
    - [as a Stream](#as-a-Stream)
      - [getFPS](#getFPS)
      - [isDropFrame](#isDropFrame)
      - [setJumpFrames]($setJumpFrames)
  - [Testing](#Testing)
  - [Contributing](#contributing)
  - [Credits](#credits)
  - [Built With](#built-with)
  - [Authors](#authors)
  - [License](#license)

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

## Installation

**BEFORE YOU INSTALL:** please read the [prerequisites](#prerequisites)

To install and set up the library, run:

```sh
$ npm install -S libltc-napi-limited
```

Or if you prefer using Yarn:

```sh
$ yarn add --dev libltc-napi-limited
```

Once the module is `install` ed, it will compile/build the native add-on for your system.


## Usage

### decodeStream

`decodeStream` expects an audio buffer in (from a Stream, such as the `naudiodon` module outputs), and returns an array of timecode strings. The size of the array will vary depending on the length of the buffer that is input. 

Note that it returns the results of the conversion (from audio buffer to timecode string) after the full buffer is converted - i.e. the first timecode string in the array will be delayed by at least array.length frames of LTC.

```js
const libltc = require('libltc-napi-limited');
const portAudio = require('naudiodon');

function main() {
    const decoder = new libltc();

    var ai = new portAudio.AudioIO({
        inOptions: {
            channelCount: 1,
            sampleFormat: portAudio.SampleFormat16Bit,
            sampleRate: 44100,
            deviceId: 1,
            closeOnError: true
        }
    });

    let result_queue, i;
    ai.on('data', buf => {
        result_queue = decoder.decodeStream(buf);
        for (i=0; i<result_queue.length; i++)
            console.log(result_queue[i]);
    });

    ai.start();
}
```

### decodeFile

`decodeFile` decodes a given file (to standard output), assuming it's raw audio (not WAV, or any other format).


```js
const libltc = require('libltc-napi-limited');

function main() {
    const decoder = new libltc();
    console.log(decoder.decodeFile("rawAudio.raw"));
}
```

### as a Stream

When instatiated, `libltc-napi-limited` returns a NodeJS [Transform Stream](https://nodejs.org/api/stream.html#duplex-and-transform-streams). Audio can then be `pipe`d into the Stream, and it will output timecode, which can be picked up via the usual methods for reading a Stream.

There are additional methods available when running as a Stream:

#### getFPS

`getFps` returns the frames-per-second of the LTC being decoded. For the first second, it will return 0 (while fps is being determined).

#### isDropFrame

`isDropFrame` returns true if the decoded timecode is drop-frame, false if not.

#### setJumpFrames

`libltc-napi-limited` decodes LTC from audio with some latency. `setJumpFrames` allows you to jump forward a certain numebr of frames to offset this latency. You cannot jump forward more than 1s.

## Testing

There are three testing files:

- `test_file.js` - this tests file conversion
- `test_pipe.js` - this tests incoming audio using `libltc-napi-limited` as a Stream
- `test_stream.js` - this tests decoding incoming audio using `decodeStream()`

To test LTC streaming in MacOS, I used [Qlab](https://qlab.app/) to send LTC to an audio loopback device ([BlackHole](https://existential.audio/blackhole/)), then pulled audio from the loopback device using the Node module `naudiodon`.

* [Qlab](https://qlab.app/)
* [BlackHole](https://existential.audio/blackhole/)

To test LTC streaming in Windows 10, I used [Audacity](https://www.audacityteam.org/) to send LTC audio from a timecode audio file to an audio loopback driver ([VB-CABLE](https://vb-audio.com/Cable/index.htm)), and pulled audio from the device using `naudiodon`. Note the output volume on Audacity had to be about 15% to avoid clipping - this will depend on the LTC audio file you use.

## <a name="contributing"></a>Contributing, Support, and Further development

This module may or may not be further developed. I will likely never work on encoding LTC from other data, or on generating it - so if you're interested in that, please fork or create a new branch and issue a pull request. 

I *am not* a C coder - so the C/C++ parts of the module are hacked together based on code snippets and example code from the libltc website and across the web. There may be more streamlined solutions to what I'm doing. I've just never found them.

Note that when decoding from a stream, libltc doesn't return data until the current buffer is decoded (about 6-8 frames) - so there's a small delay; this is a bit of a killer.

Things I'd like to do:

* [todo] more error correction

## Credits

* [libltc](https://x42.github.io/libltc/index.html)

## Built With

* Yeoman

## Authors

* **Simon Harding** - *Initial work* - [simon-shd](https://github.com/simon-shd)

## License

Since libltc is released under the LGPLv3 license, I believe this module must be released under the same license, even though no modifications were made to the underlying libltc code. I think any software created using this module doesn't have to be LGPLv3, since the module can be replaced by the end user. **I'm not a legal expert, so read up on the licenses and make your own conclusions.**

[LGPLv3 License](http://www.gnu.org/licenses/lgpl-3.0.txt)