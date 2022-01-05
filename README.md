[![code style: prettier](https://img.shields.io/badge/code_style-prettier-ff69b4.svg?style=flat-square)](https://github.com/prettier/prettier)


# <a name="project-name"></a>Libltc NAPI (Limited)

> libltc-napi-limited is a limited (decode-only) implementation of the libltc C library in NodeJS, using node-addon-api. It can decode a raw audio file, or an audio stream.

(from the Libltc website):

Linear (or Longitudinal) Timecode (LTC) is an encoding of timecode data as a Manchester-Biphase encoded 
audio signal. The audio signal is commonly recorded on a VTR track or other storage media. 

The libltc C library provides functionality to encode and decode LTC from/to timecode, including SMPTE date support.


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


## Table of contents

- [LibLTC NAPI (Limited)](#project-name)
  - [Prerequisites](#prerequisites)
  - [Table of contents](#table-of-contents)
  - [Getting Started](#getting-started)
  - [Installation](#installation)
  - [Usage](#usage)
    - [decodeStream](#decodeStream)
    - [decodeFile](#decodeFile)
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

`decodeFile` decodes a given file, assuming it's raw audio (not WAV, or any other format).


```js
const libltc = require('libltc-napi-limited');

function main() {
    const decoder = new libltc();
    console.log(decoder.decodeFile("rawAudio.raw"));
}
```

## Testing

To test LTC streaming, I used [Qlab](https://qlab.app/) to send LTC to an audio loopback device ([BlackHole](https://existential.audio/blackhole/)), then used this test to pull audio from the loopback device.

* [Qlab](https://qlab.app/)
* [BlackHole](https://existential.audio/blackhole/)


## <a name="contributing"></a>Contributing, Support, and Further development

This module may or may not be further developed. I will likely never work on encoding LTC from other data, or on generating it - so if you're interested in that, please fork or create a new branch and issue a pull request. 

I *am not* a C coder - so the C/C++ parts of the module are hacked together based on code snippets and example code from the libltc website and across the web. There may be more streamlined solutions to what I'm doing. I've just never found them.

Note that when decoding from a stream, libltc doesn't return data until the current buffer is decoded (about 6-8 frames) - so there's a small delay; this is a bit of a killer.

Things I'd like to do:

* [todo] expose the decodeStream functionality as a Stream, so an audio stream can be `pipe`d into it, and it `emit`s data as timecode whenever a frame is decoded.
* [todo] more error correction

## Credits

* [libltc](https://x42.github.io/libltc/index.html)

## Built With

* Yeoman

## Authors

* **Simon Harding** - *Initial work* - [simon-shd](https://github.com/simon-shd)

## License

Since libltc is released under the LGPLv3 license, I believe this module must be released under the same license, even though no modifications were made to the underlying libltc code. I think any software created using this module doesn't have to be LGPLv3, since the module can be replaced by the end user. (but I'm not a legal scholar, so read up on the licenses and make your own conclusions).

[LGPLv3 License](http://www.gnu.org/licenses/lgpl-3.0.txt)