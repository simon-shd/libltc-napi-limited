#pragma once

#include <napi.h>

class LibltcNapiLimited : public Napi::ObjectWrap<LibltcNapiLimited>
{
public:
    LibltcNapiLimited(const Napi::CallbackInfo&);
    Napi::Value DecodeFile(const Napi::CallbackInfo&);
    Napi::Value DecodeStream(const Napi::CallbackInfo&);
    Napi::Value DecodeChunk(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);

private:
};
