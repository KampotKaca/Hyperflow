#ifndef UNIFORMS_H
#define UNIFORMS_H

#include <hyperflow.h>

namespace app
{
    struct GlobalUniformInfo
    {
        hf::CameraUniformInfo camera{};
        hf::TimeUniformInfo time{};
    };

    struct AppUniforms
    {
        GlobalUniformInfo globalUniformInfo{};
    };

    extern AppUniforms APP_UNIFORMS;

    void UniformDefineAll();
    void UniformStartAll();

    void UniformUploadAll(const hf::Ref<hf::Renderer>& rn);
}

#endif //UNIFORMS_H
