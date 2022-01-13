#include "libltc_napi_limited.h"

#include <stdio.h>
#include <math.h>
#include "libltc/ltc.h"

#ifdef _WIN32
#include <fcntl.h> // for _fmode
#endif

#define BUFFER_SIZE (1024)

using namespace Napi;

LibltcNapiLimited::LibltcNapiLimited(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();

    // this is audio samples-per-video frames, which can generally be calculated
    // as [audio sample rate] / [framerate]. The default from libltc's implementation
    // seems to be 48000Hz / 25fps = 1920. I changed it to 44100 / 30 = 1470. Libltc
    // calculates on the fly, so it doesn't really matter much.
    int apv = 1470;

    this->_decoder = ltc_decoder_create(apv, 32);
}

Napi::Value LibltcNapiLimited::DecodeFile(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    // Napi::String filename = info[0].As<Napi::String>();

    int apv = 1920;
    ltcsnd_sample_t sound[BUFFER_SIZE];
    size_t n;
    long int total;
    FILE *f;
    const char *filename = info[0].As<Napi::String>().Utf8Value().c_str();

    LTCDecoder *decoder;
    LTCFrameExt frame;

#ifdef _WIN32
    // see https://msdn.microsoft.com/en-us/library/ktss1a9b.aspx and
    // https://github.com/x42/libltc/issues/18
    _set_fmode(_O_BINARY);
#endif

    f = fopen(filename, "r");

    printf("Filename is %s\n", filename);

    if (!f)
    {
        fprintf(stderr, "error opening '%s'\n", filename);
    }
    fprintf(stderr, "* reading from: %s\n", filename);

    total = 0;

    decoder = ltc_decoder_create(apv, 32);

    do
    {
        n = fread(sound, sizeof(ltcsnd_sample_t), BUFFER_SIZE, f);
        ltc_decoder_write(decoder, sound, n, total);

        while (ltc_decoder_read(decoder, &frame))
        {
            SMPTETimecode stime;

            ltc_frame_to_time(&stime, &frame.ltc, 1);

            printf("%04d-%02d-%02d %s ",
                   ((stime.years < 67) ? 2000 + stime.years : 1900 + stime.years),
                   stime.months,
                   stime.days,
                   stime.timezone);

            printf("%02d:%02d:%02d%c%02d | %8lld %8lld%s\n",
                   stime.hours,
                   stime.mins,
                   stime.secs,
                   (frame.ltc.dfbit) ? '.' : ':',
                   stime.frame,
                   frame.off_start,
                   frame.off_end,
                   frame.reverse ? "  R" : "");
        }

        total += n;

    } while (n);

    fclose(f);
    ltc_decoder_free(decoder);

    return Napi::String::New(env, "File Decoded?");
}

Napi::Value LibltcNapiLimited::DecodeStream(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    // interpret the first argument as a NodeJS Buffer, cast it to a char, and
    // then use the node-addon-api method Data() to get a pointer to the data
    char *stream = info[0].As<Napi::Buffer<char>>().Data();

    // use the node-addon-api method Length() to get the length of the buffer
    int buffer_size = info[0].As<Napi::Buffer<char>>().Length();

    // instantiate an unsigned char array of the size of the buffer
    // note ltcsnd_sample_t is a libltc type for audio data
    // this char array will contain audio values from stream
    ltcsnd_sample_t *sound = new ltcsnd_sample_t[buffer_size];

    // now we have to convert the pointer data from a char (seems to come in
    // signed), back to an unsigned char array so libltc can process it. If node-addon-api
    // allowed us to cast to an unsigned char, that would be great - but since
    // it doesn't we have to do it manually
    int a;
    for (a = 0; a < buffer_size; a++)
    {
        sound[a] = static_cast<unsigned char>(*stream); // this is a C++ cast from signed to unsigned
        stream++;                                       // this moves the pointer forward by one byte (the size of its type, char)
    }

    int apv = 1470;

    LTCDecoder *decoder;
    LTCFrameExt frame;

    decoder = ltc_decoder_create(apv, 32);
    ltc_decoder_write(decoder, sound, buffer_size, 0);

    Napi::Array timecode_queue = Napi::Array::New(env);
    uint32_t i = 0;

    while (ltc_decoder_read(decoder, &frame))
    {
        SMPTETimecode stime;

        ltc_frame_to_time(&stime, &frame.ltc, 1);

        char tc_str[11];
        sprintf(tc_str, "%02d:%02d:%02d%c%02d",
                stime.hours,
                stime.mins,
                stime.secs,
                (frame.ltc.dfbit) ? '.' : ':',
                stime.frame);

        timecode_queue[i++] = Napi::String::New(env, tc_str);
    }

    ltc_decoder_free(decoder);

    return timecode_queue;
}

Napi::Value LibltcNapiLimited::DecodeChunk(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    // interpret the first argument as a NodeJS Buffer, cast it to a char, and
    // then use the node-addon-api method Data() to get a pointer to the data
    char *stream = info[0].As<Napi::Buffer<char>>().Data();

    // use the node-addon-api method Length() to get the length of the buffer
    int buffer_size = info[0].As<Napi::Buffer<char>>().Length();

    // get the callback function that will push decoded values into the Stream
    Napi::Function cb = info[1].As<Napi::Function>();

    // instantiate an unsigned char array of the size of the buffer
    // note ltcsnd_sample_t is a libltc type for audio data
    // this char array will contain audio values from stream
    ltcsnd_sample_t *sound = new ltcsnd_sample_t[buffer_size];

    // now we have to convert the pointer data from a char (seems to come in
    // signed), back to an unsigned char array so libltc can process it. If node-addon-api
    // allowed us to cast to an unsigned char, that would be great - but since
    // it doesn't we have to do it manually
    int a;
    for (a = 0; a < buffer_size; a++)
    {
        sound[a] = static_cast<unsigned char>(*stream); // this is a C++ cast from signed to unsigned
        stream++;                                       // this moves the pointer forward by one byte (the size of its type, char)
    }

    LTCFrameExt frame;

    ltc_decoder_write(this->_decoder, sound, buffer_size / sizeof(ltcsnd_sample_t), 0);

    while (ltc_decoder_read(this->_decoder, &frame))
    {
        SMPTETimecode stime;

        ltc_frame_to_time(&stime, &frame.ltc, 1);

        char tc_str[11];
        sprintf(tc_str, "%02d:%02d:%02d%c%02d",
                stime.hours,
                stime.mins,
                stime.secs,
                (frame.ltc.dfbit) ? '.' : ':',
                stime.frame);

        cb.Call(env.Global(), {Napi::String::New(env, tc_str)});
    }

    // the function returns an Object with a boolean property of "dropFrame", whether
    // the TC is drop frame or not.
    Napi::Object return_opt = Napi::Object::New(env);
    return_opt.Set(Napi::String::New(env, "dropFrame"), (frame.ltc.dfbit));

    return return_opt;
}

void LibltcNapiLimited::FreeDecoder(const Napi::CallbackInfo &info)
{
    ltc_decoder_free(this->_decoder);
}

Napi::Function LibltcNapiLimited::GetClass(Napi::Env env)
{
    return DefineClass(
        env,
        "LibltcNapiLimited",
        {
            LibltcNapiLimited::InstanceMethod("decodeFile", &LibltcNapiLimited::DecodeFile),
            LibltcNapiLimited::InstanceMethod("decodeStream", &LibltcNapiLimited::DecodeStream),
            LibltcNapiLimited::InstanceMethod("decodeChunk", &LibltcNapiLimited::DecodeChunk),
            LibltcNapiLimited::InstanceMethod("freeDecoder", &LibltcNapiLimited::FreeDecoder),
        });
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "LibltcNapiLimited");
    exports.Set(name, LibltcNapiLimited::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
