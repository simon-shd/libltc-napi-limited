#pragma once

#include <napi.h>
#include "libltc/ltc.h"

class LibltcNapiLimited : public Napi::ObjectWrap<LibltcNapiLimited>
{
public:
    LibltcNapiLimited(const Napi::CallbackInfo&);
    Napi::Value DecodeFile(const Napi::CallbackInfo&);
    Napi::Value DecodeStream(const Napi::CallbackInfo&);
    Napi::Value DecodeChunk(const Napi::CallbackInfo&);
    void FreeDecoder(const Napi::CallbackInfo &info);

    static Napi::Function GetClass(Napi::Env);

private:
    LTCDecoder * _decoder;
};
